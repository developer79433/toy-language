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
    return &stack->frame;
}

const interp_frame *interp_stack_payload_const(const interp_stack *stack)
{
    return &stack->frame;
}

void interp_stack_dump(FILE *f, const char *context, const interp_stack *stack)
{
    return interp_frame_stack_dump(f, context, stack);
}

void interp_stack_free(interp_stack *stack)
{
    return interp_frame_stack_free(stack);
}

static interp_frame_stack *alloc_frame_loop(const toy_block *loop_body)
{
    interp_frame_stack *stack = mymalloc(interp_frame_stack);
    stack->prev = NULL;
    interp_frame *frame = interp_frame_stack_payload(stack);
    frame->type = FRAME_LOOP_BODY;
    frame->loop_body = loop_body;
    frame->cur_stmt = loop_body->stmts;
    symbol_table_init(&frame->symbols);
    return stack;
}

static interp_frame_stack *alloc_frame_if(const toy_block *if_body)
{
    interp_frame_stack *stack = mymalloc(interp_frame_stack);
    stack->prev = NULL;
    interp_frame *frame = interp_frame_stack_payload(stack);
    frame->type = FRAME_IF_BODY;
    frame->if_body = if_body;
    frame->cur_stmt = if_body->stmts;
    symbol_table_init(&frame->symbols);
    return stack;
}

static interp_frame_stack *alloc_frame_block(const toy_block *block)
{
    interp_frame_stack *stack = mymalloc(interp_frame_stack);
    stack->prev = NULL;
    interp_frame *frame = interp_frame_stack_payload(stack);
    frame->type = FRAME_BLOCK_STMT;
    frame->block_stmt = block;
    frame->cur_stmt = block->stmts;
    symbol_table_init(&frame->symbols);
    return stack;
}

static interp_frame_stack *alloc_frame_user_func(const toy_function *func)
{
    assert(func->type == FUNC_USER_DECLARED);
    interp_frame_stack *stack = mymalloc(interp_frame_stack);
    stack->prev = NULL;
    interp_frame *frame = interp_frame_stack_payload(stack);
    frame->type = FRAME_USER_DEF_FUNC;
    frame->user_def_func = func;
    frame->cur_stmt = func->code.stmts;
    symbol_table_init(&frame->symbols);
    symbol_table_assert_valid(&frame->symbols);
    return stack;
}

static interp_frame_stack *alloc_frame_predef_func(const toy_function *func)
{
    assert(func->type == FUNC_PREDEFINED);
    interp_frame_stack *stack = mymalloc(interp_frame_stack);
    stack->prev = NULL;
    interp_frame *frame = interp_frame_stack_payload(stack);
    frame->type = FRAME_PRE_DEF_FUNC;
    frame->pre_def_func = func;
    frame->cur_stmt = func->code.stmts;
    symbol_table_init(&frame->symbols);
    return stack;
}

interp_stack *interp_stack_push_if(interp_stack *stack, const toy_block *block)
{
    /* FIXME: Because this decorates its argument, it needs to allocate new storage */
    interp_frame_stack *new_stack = alloc_frame_if(block);
    stack = interp_frame_stack_push(stack, new_stack);
    interp_frame_stack_dump(stderr, "after push if body", stack);
    return stack;
}

interp_stack *interp_stack_push_loop(interp_stack *stack, const toy_block *block)
{
    interp_frame_stack *new_stack = alloc_frame_loop(block);
    stack = interp_frame_stack_push(stack, new_stack);
    interp_frame_stack_dump(stderr, "after push loop body", stack);
    return stack;
}

interp_stack *interp_stack_push_predef_func(interp_stack *stack, const toy_function *func)
{
    assert(FUNC_PREDEFINED == func->type);
    interp_frame_stack *new_stack = alloc_frame_predef_func(func);
    stack = interp_frame_stack_push(stack, new_stack);
    interp_frame_stack_dump(stderr, "after push predef func", stack);
    return stack;
}

interp_stack *interp_stack_push_user_func(interp_stack *stack, const toy_function *func)
{
    assert(FUNC_USER_DECLARED == func->type);
    interp_frame_stack *new_stack = alloc_frame_user_func(func);
    stack = interp_frame_stack_push(stack, new_stack);
    interp_frame_stack_dump(stderr, "after push user func", stack);
    return stack;
}

interp_stack *interp_stack_push_block(interp_stack *stack, const toy_block *block)
{
    interp_frame_stack *new_stack = alloc_frame_block(block);
    stack = interp_frame_stack_push(stack, new_stack);
    interp_frame_stack_dump(stderr, "after push block stmt", stack);
    return stack;
}

interp_stack *interp_stack_pop(interp_stack *stack)
{
    return interp_frame_stack_pop(stack);
}

get_result lookup_user_identifier(interp_stack *stack, toy_val *result, toy_str name)
{
    assert(!lookup_predefined_constant(name));
    assert(!predef_func_lookup_name(name));
    /* TODO: Use interp_stack_foreach */
    for (; stack; stack = stack->prev) {
        interp_frame *frame = interp_stack_payload(stack);
        get_result found = lookup_identifier_in_frame(frame, result, name);
        if (found == GET_FOUND) {
            return GET_FOUND;
        }
    }
    return GET_NOT_FOUND;
}
