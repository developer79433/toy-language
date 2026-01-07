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

static void interp_stack_assert_valid(const interp_stack *stack)
{
    /* TODO */
}

size_t interp_stack_len(const interp_stack *stack)
{
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

static void init_variables(const toy_block *block, size_t *num_variables, toy_val **variables)
{
    size_t count = count_variables(block);
    if (count) {
        *num_variables = count;
        *variables = mymalloc_array(toy_val, count);
    } else {
        *num_variables = 0;
        *variables = NULL;
    }
}

interp_stack *interp_stack_push_if(interp_stack *stack, const toy_block *block)
{
    interp_stack_assert_valid(stack);
    size_t num_variables;
    toy_val *variables;
    init_variables(block, &num_variables, &variables);
    interp_frame frame = { .type = FRAME_IF_BODY, .block_stmt.block = block, .variables = variables, .num_variables = num_variables, .cur_stmt = block->stmts, .cur_val = variables };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push if body", stack);
    return stack;
}

interp_stack *interp_stack_push_loop(interp_stack *stack, const toy_block *block)
{
    interp_stack_assert_valid(stack);
    size_t num_variables;
    toy_val *variables;
    init_variables(block, &num_variables, &variables);
    interp_frame frame = { .type = FRAME_LOOP_BODY, .block_stmt.block = block, .variables = variables, .num_variables = num_variables, .cur_stmt = block->stmts, .cur_val = variables };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push loop body", stack);
    return stack;
}

static item_callback_result copy_arg_callback(void *cookie, size_t index, const toy_val_list *item)
{
    toy_val *frame_args = (toy_val *) cookie;
    const toy_val *actual_argument = val_list_payload_const(item);
    memcpy(&frame_args[index], actual_argument, sizeof(frame_args[index]));
    return CONTINUE_ENUMERATION;
}

static void copy_args_into_frame(toy_val *frame_args, const toy_val_list *actual_arguments)
{
    enumeration_result res = val_list_foreach_const(actual_arguments, copy_arg_callback, frame_args);
    assert(ENUMERATION_COMPLETE == res);
}

interp_stack *interp_stack_push_predef_func(interp_stack *stack, const toy_function *func, const toy_val_list *actual_arguments)
{
    interp_stack_assert_valid(stack);
    assert(FUNC_PREDEFINED == func->type);
    size_t args_len = val_list_len(actual_arguments);
    assert(args_len == str_list_len(func->param_names));
    toy_val *arguments;
    if (args_len) {
        arguments = mymalloc_array(toy_val, args_len);
    } else {
        arguments = NULL;
    }
    copy_args_into_frame(arguments, actual_arguments);
    interp_frame frame = { .type = FRAME_PRE_DEF_FUNC, .variables = NULL, .num_variables = 0, .func_call.func = func, .func_call.arguments = arguments, .func_call.num_arguments = args_len, .cur_stmt = func->code.stmts, .cur_val = NULL };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push predef func", stack);
    return stack;
}

interp_stack *interp_stack_push_user_func(interp_stack *stack, const toy_function *func, const toy_val_list *actual_arguments)
{
    interp_stack_assert_valid(stack);
    assert(FUNC_USER_DECLARED == func->type);
    size_t args_len = val_list_len(actual_arguments);
    assert(args_len == str_list_len(func->param_names));
    toy_val *arguments;
    if (args_len) {
        arguments = mymalloc_array(toy_val, args_len);
    } else {
        arguments = NULL;
    }
    copy_args_into_frame(arguments, actual_arguments);
    size_t num_variables;
    toy_val *variables;
    init_variables(&func->code, &num_variables, &variables);
    interp_frame frame = {.type = FRAME_USER_DEF_FUNC, .variables = variables, .num_variables = num_variables, .func_call.func = func, .func_call.arguments = arguments, .func_call.num_arguments = args_len, .cur_stmt = func->code.stmts, .cur_val = variables };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push user func", stack);
    return stack;
}

interp_stack *interp_stack_push_block(interp_stack *stack, const toy_block *block)
{
    interp_stack_assert_valid(stack);
    size_t num_variables;
    toy_val *variables;
    init_variables(block, &num_variables, &variables);
    interp_frame frame = { .type = FRAME_BLOCK_STMT, .variables = variables, .num_variables = num_variables, .block_stmt.block = block, .cur_stmt = block->stmts, .cur_val = variables };
    stack = interp_frame_stack_push(stack, &frame);
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
    return interp_frame_stack_index(stack, index);
}

const interp_frame *interp_stack_index_const(const interp_stack *stack, size_t index)
{
    interp_stack_assert_valid(stack);
    assert(index <= interp_stack_len(stack));
    return interp_frame_stack_index_const(stack, index);
}

toy_val *interp_stack_get_func_param(interp_stack *stack, size_t frames_up, size_t param_index)
{
    interp_stack_assert_valid(stack);
    interp_frame *frame = interp_stack_index(stack, frames_up);
    return interp_frame_get_func_param(frame, param_index);
}
