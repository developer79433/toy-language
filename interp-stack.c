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

void interp_stack_dump(const char *context, const interp_stack *stack)
{
    return interp_frame_stack_dump(context, stack);
}

void interp_stack_free(interp_stack *stack)
{
    return interp_frame_stack_free(stack);
}

interp_stack *interp_stack_push_if(interp_stack *stack, const toy_block *block)
{
    toy_val *variables = NULL; /* TODO */
    interp_frame frame = { .type = FRAME_IF_BODY, .block_stmt.block = block, .variables = variables, .cur_stmt = block->stmts };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push if body", stack);
    return stack;
}

interp_stack *interp_stack_push_loop(interp_stack *stack, const toy_block *block)
{
    toy_val *variables = NULL; /* TODO */
    interp_frame frame = { .type = FRAME_LOOP_BODY, .block_stmt.block = block, .variables = variables, .cur_stmt = block->stmts };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push loop body", stack);
    return stack;
}

interp_stack *interp_stack_push_predef_func(interp_stack *stack, const toy_function *func, const toy_val_list *args)
{
    assert(FUNC_PREDEFINED == func->type);
    interp_frame frame = { .type = FRAME_PRE_DEF_FUNC, .func_call.func = func, .func_call.args = args, .cur_stmt = func->code.stmts };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push predef func", stack);
    return stack;
}

interp_stack *interp_stack_push_user_func(interp_stack *stack, const toy_function *func, const toy_val_list *args)
{
    assert(FUNC_USER_DECLARED == func->type);
    interp_frame frame = {.type = FRAME_USER_DEF_FUNC, .func_call.func = func, .func_call.args = args, .cur_stmt = func->code.stmts };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push user func", stack);
    return stack;
}

interp_stack *interp_stack_push_block(interp_stack *stack, const toy_block *block)
{
    toy_val *variables = NULL; /* TODO */
    interp_frame frame = { .type = FRAME_BLOCK_STMT, .block_stmt.block = block, .variables = variables, .cur_stmt = block->stmts };
    stack = interp_frame_stack_push(stack, &frame);
    interp_frame_stack_dump("after push block stmt", stack);
    return stack;
}

interp_stack *interp_stack_pop(interp_stack *stack)
{
    return interp_frame_stack_pop(stack, NULL);
}

interp_stack *interp_stack_index(interp_stack *stack, size_t index)
{
    return interp_frame_stack_index(stack, index);
}

const interp_stack *interp_stack_index_const(const interp_stack *stack, size_t index)
{
    return interp_frame_stack_index_const(stack, index);
}

toy_val *interp_stack_get_variable(interp_stack *stack, size_t frames_up, size_t var_index)
{
    interp_stack *item = interp_stack_index(stack, frames_up);
    interp_frame *frame = interp_stack_payload(item);
    return interp_frame_get_var(frame, var_index);
}
