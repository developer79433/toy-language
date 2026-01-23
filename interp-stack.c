#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "interp-stack.h"
#include "interp-frame-stack.h"
#include "mymalloc.h"
#include "symbol-table.h"
#include "predef-function.h"
#include "constants.h"
#include "interp-frame.h"
#include "val-list.h"
#include "str-list.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "block.h"
#include "errors.h"
#include "log.h"
#include "var.h"
#include "function.h"

void interp_stack_assert_valid(const interp_stack *stack)
{
    interp_frame_stack_assert_valid(stack);
}

size_t interp_stack_len(const interp_stack *stack)
{
    interp_stack_assert_valid(stack);
    return interp_frame_stack_len(stack);
}

interp_frame *interp_stack_payload(interp_stack *stack)
{
    interp_stack_assert_valid(stack);
    return interp_frame_stack_payload(stack);
}

const interp_frame *interp_stack_payload_const(const interp_stack *stack)
{
    interp_stack_assert_valid(stack);
    return interp_frame_stack_payload_const(stack);
}

void interp_stack_dump(const char *context, const interp_stack *stack)
{
    interp_stack_assert_valid(stack);
    return interp_frame_stack_dump(context, stack);
}

void interp_stack_free(interp_stack *stack)
{
    interp_stack_assert_valid(stack);
    return interp_frame_stack_free(stack);
}

static toy_var *init_variables(const toy_block *block, size_t *num_declarations)
{
    *num_declarations = block_num_declarations(block);
    if (*num_declarations) {
        return var_alloc_array(*num_declarations);
    }
    return NULL;
}

interp_stack *interp_stack_push_if(interp_stack *stack, const toy_block *block)
{
    interp_stack_assert_valid(stack);
    size_t num_variables;
    toy_var *variables = init_variables(block, &num_variables);
    interp_frame frame = {
        .type = FRAME_IF_BODY,
        .block_stmt.block = block,
        .variables = variables,
        .num_variables = num_variables,
        .cur_stmt = block->stmts
    };
    stack = interp_frame_stack_push(stack, &frame);
    interp_stack_assert_valid(stack);
    interp_frame_stack_dump("after push if body", stack);
    return stack;
}

interp_stack *interp_stack_push_loop(interp_stack *stack, const toy_block *block)
{
    interp_stack_assert_valid(stack);
    size_t num_variables;
    toy_var *variables = init_variables(block, &num_variables);
    interp_frame frame = {
        .type = FRAME_LOOP_BODY,
        .block_stmt.block = block,
        .variables = variables,
        .num_variables = num_variables,
        .cur_stmt = block->stmts
    };
    stack = interp_frame_stack_push(stack, &frame);
    interp_stack_assert_valid(stack);
    interp_frame_stack_dump("after push loop body", stack);
    return stack;
}

typedef struct copy_arg_args_struct {
    toy_var *frame_args;
} copy_arg_args;

static item_callback_result copy_arg_callback(void *cookie, size_t index, const toy_val_list *item)
{
    copy_arg_args *args = (copy_arg_args *) cookie;
    const toy_val *actual_argument = val_list_payload_const(item);
    toy_var *var = &args->frame_args[index];
    /* FIXME: const poisoning */
    /* FIXME: Lack of distinction between const list of values, and list of const values */
    var_set(var, (toy_val *) actual_argument);
    return CONTINUE_ENUMERATION;
}

static void copy_args_into_frame(toy_var *frame_args, const toy_val_list *actual_arguments)
{
    copy_arg_args args = { .frame_args = frame_args };
    enumeration_result res = val_list_foreach_const(actual_arguments, copy_arg_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
}

interp_stack *interp_stack_push_predef_func(interp_stack *stack, const toy_function *func, const toy_val_list *actual_arguments)
{
    interp_stack_assert_valid(stack);
    assert(FUNC_PREDEFINED == func->type);
    size_t args_len = val_list_len(actual_arguments);
    assert(func->param_names == &INFINITE_PARAMS || args_len == str_list_len(func->param_names));
    toy_var *arguments;
    if (args_len) {
        arguments = var_alloc_array(args_len);
    } else {
        arguments = NULL;
    }
    copy_args_into_frame(arguments, actual_arguments);
    interp_frame frame = {
        .type = FRAME_PRE_DEF_FUNC,
        .variables = NULL,
        .num_variables = 0,
        .func_call.func = func,
        .func_call.arguments = arguments,
        .func_call.num_arguments = args_len,
        .cur_stmt = func->code->stmts
    };
    stack = interp_frame_stack_push(stack, &frame);
    interp_stack_assert_valid(stack);
    interp_frame_stack_dump("after push predef func", stack);
    return stack;
}

interp_stack *interp_stack_push_user_func(interp_stack *stack, const toy_function *func, const toy_val_list *actual_arguments)
{
    interp_stack_assert_valid(stack);
    assert(FUNC_USER_DECLARED == func->type);
    size_t args_len = val_list_len(actual_arguments);
    assert(args_len == str_list_len(func->param_names));
    toy_var *arguments;
    if (args_len) {
        arguments = var_alloc_array(args_len);
    } else {
        arguments = NULL;
    }
    copy_args_into_frame(arguments, actual_arguments);
    size_t num_variables;
    toy_var *variables = init_variables(func->code, &num_variables);
    interp_frame frame = {
        .type = FRAME_USER_DEF_FUNC,
        .variables = variables,
        .num_variables = num_variables,
        .func_call.func = func,
        .func_call.arguments = arguments,
        .func_call.num_arguments = args_len,
        .cur_stmt = func->code->stmts
    };
    stack = interp_frame_stack_push(stack, &frame);
    interp_stack_assert_valid(stack);
    interp_frame_stack_dump("after push user func", stack);
    return stack;
}

interp_stack *interp_stack_push_block(interp_stack *stack, const toy_block *block)
{
    interp_stack_assert_valid(stack);
    size_t num_variables;
    toy_var *variables = init_variables(block, &num_variables);
    interp_frame frame = {
        .type = FRAME_BLOCK_STMT,
        .variables = variables,
        .num_variables = num_variables,
        .block_stmt.block = block,
        .cur_stmt = block->stmts
    };
    stack = interp_frame_stack_push(stack, &frame);
    interp_stack_assert_valid(stack);
    interp_frame_stack_dump("after push block stmt", stack);
    return stack;
}

interp_stack *interp_stack_pop(interp_stack *stack)
{
    interp_stack_assert_valid(stack);
    return interp_frame_stack_pop(stack, NULL);
}

interp_frame *interp_stack_index(interp_stack *stack, size_t index)
{
    interp_stack_assert_valid(stack);
    assert(index <= interp_stack_len(stack));
    interp_frame *frame = interp_frame_stack_index(stack, index);
    interp_frame_assert_valid(frame);
    return frame;
}

const interp_frame *interp_stack_index_const(const interp_stack *stack, size_t index)
{
    interp_stack_assert_valid(stack);
    assert(index <= interp_stack_len(stack));
    const interp_frame *frame = interp_frame_stack_index_const(stack, index);
    interp_frame_assert_valid(frame);
    return frame;
}
