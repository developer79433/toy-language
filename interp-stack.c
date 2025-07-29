#include <assert.h>
#include <stdio.h>

#include "interp-stack.h"
#include "interp-frame-stack.h"
#include "mymalloc.h"
#include "symbol-table.h"
#include "predef-function.h"
#include "constants.h"
#include "interp-frame.h"

interp_frame *interp_stack_payload(interp_stack *stack)
{
    return interp_frame_stack_payload(stack);
}

const interp_frame *interp_stack_payload_const(const interp_stack *stack)
{
    return interp_frame_stack_payload_const(stack);
}

void interp_stack_dump(FILE *f, const char *context, const interp_stack *stack)
{
    return interp_frame_stack_dump(f, context, stack);
}

void interp_stack_free(interp_stack *stack)
{
    return interp_frame_stack_free(stack);
}

interp_stack *interp_stack_push_if(interp_stack *stack, const toy_block *block)
{
    interp_frame frame = { .type = FRAME_IF_BODY, .if_body = block, .cur_stmt = block->stmts };
    symbol_table_init(&frame.symbols);
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump(stderr, "after push if body", stack);
    return stack;
}

interp_stack *interp_stack_push_loop(interp_stack *stack, const toy_block *block)
{
    interp_frame frame = { .type = FRAME_LOOP_BODY, .loop_body = block, .cur_stmt = block->stmts };
    symbol_table_init(&frame.symbols);
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump(stderr, "after push loop body", stack);
    return stack;
}

interp_stack *interp_stack_push_predef_func(interp_stack *stack, const toy_function *func)
{
    assert(FUNC_PREDEFINED == func->type);
    interp_frame frame = { .type = FRAME_PRE_DEF_FUNC, .pre_def_func = func, .cur_stmt = func->code.stmts };
    symbol_table_init(&frame.symbols);
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump(stderr, "after push predef func", stack);
    return stack;
}

interp_stack *interp_stack_push_user_func(interp_stack *stack, const toy_function *func)
{
    assert(FUNC_USER_DECLARED == func->type);
    interp_frame frame = {.type = FRAME_USER_DEF_FUNC, .user_def_func = func, .cur_stmt = func->code.stmts };
    symbol_table_init(&frame.symbols);
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump(stderr, "after push user func", stack);
    return stack;
}

interp_stack *interp_stack_push_block(interp_stack *stack, const toy_block *block)
{
    interp_frame frame = { .type = FRAME_BLOCK_STMT, .block_stmt = block, .cur_stmt = block->stmts };
    symbol_table_init(&frame.symbols);
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump(stderr, "after push block stmt", stack);
    return stack;
}

interp_stack *interp_stack_pop(interp_stack *stack)
{
    return interp_frame_stack_pop(stack, NULL);
}

typedef struct frame_lookup_cb_args_struct {
    toy_str desired_name;
    toy_val *result;
} frame_lookup_cb_args;

static item_callback_result frame_lookup_callback(void *cookie, size_t index, interp_frame_stack *stack)
{
    frame_lookup_cb_args *args = (frame_lookup_cb_args *) cookie;
    interp_frame *frame = interp_stack_payload(stack);
    get_result found = interp_frame_lookup_identifier(frame, args->result, args->desired_name);
    if (found == GET_FOUND) {
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

get_result lookup_user_identifier(interp_stack *stack, toy_val *result, toy_str name)
{
    assert(!lookup_predefined_constant(name));
    assert(!predef_func_lookup_name(name));
    frame_lookup_cb_args args = { .desired_name = name, .result = result };
    enumeration_result res = interp_frame_stack_foreach(stack, frame_lookup_callback, &args);
    if (ENUMERATION_COMPLETE == res) {
        return GET_NOT_FOUND;
    }
    return GET_FOUND;
}
