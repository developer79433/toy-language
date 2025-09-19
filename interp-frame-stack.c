#include <assert.h>
#include <stdio.h>

#include "interp-frame-stack.h"
#include "interp-frame.h"
#include "mymalloc.h"
#include "symbol-table.h"
#include "interp.h"
#include "debug.h"
#include "buf-stack.h"

typedef struct interp_frame_stack_struct {
    struct interp_frame_stack_struct *next;
    interp_frame frame;
} interp_frame_stack;

interp_frame *interp_frame_stack_payload(interp_frame_stack *stack)
{
    return &stack->frame;
}

const interp_frame *interp_frame_stack_payload_const(const interp_frame_stack *stack)
{
    return &stack->frame;
}

enumeration_result interp_frame_stack_foreach(interp_frame_stack *stack, interp_frame_stack_item_callback callback, void *cookie)
{
    return buf_stack_foreach((buf_stack *) stack, (buf_stack_item_callback) callback, cookie);
}

enumeration_result interp_frame_stack_foreach_const(const interp_frame_stack *stack, const_interp_frame_stack_item_callback callback, void *cookie)
{
    return buf_stack_foreach_const((const buf_stack *) stack, (const_buf_stack_item_callback) callback, cookie);
}

static item_callback_result frame_free_callback(void *cookie, size_t index, interp_frame_stack *stack)
{
    interp_frame *frame = interp_frame_stack_payload(stack);
    interp_frame_free(frame);
    return CONTINUE_ENUMERATION;
}

void interp_frame_stack_free(interp_frame_stack *stack)
{
    enumeration_result res = interp_frame_stack_foreach(stack, frame_free_callback, NULL);
    assert(ENUMERATION_COMPLETE == res);
}

static item_callback_result frame_assert_valid_callback(void *cookie, size_t index, const interp_frame_stack *stack)
{
    const interp_frame *frame = interp_frame_stack_payload_const(stack);
    interp_frame_assert_valid(frame);
    return CONTINUE_ENUMERATION;
}

void interp_frame_stack_assert_valid(const interp_frame_stack *stack)
{
    enumeration_result res = interp_frame_stack_foreach_const(stack, frame_assert_valid_callback, NULL);
    assert(ENUMERATION_COMPLETE == res);
}

typedef struct frame_dump_cb_args_struct {
    size_t *frame_num;
} frame_dump_cb_args;

static item_callback_result frame_dump_callback(void *cookie, size_t index, const interp_frame_stack *item)
{
    frame_dump_cb_args *args = (frame_dump_cb_args *) cookie;
    const interp_frame *frame = interp_frame_stack_payload_const(item);
    fprintf(stderr, "  Frame %02zu: ", *args->frame_num);
    interp_frame_dump(frame);
    fprintf(stderr, "\n");
    *args->frame_num = *args->frame_num + 1;
    return CONTINUE_ENUMERATION;
}

void interp_frame_stack_dump(const char *context, const interp_frame_stack *stack)
{
    fprintf(stderr, "Interpreter stack %s:\n", context);
    size_t frame_num = 0;
    frame_dump_cb_args args = { .frame_num = &frame_num };
    enumeration_result res = interp_frame_stack_foreach_const(stack, frame_dump_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

interp_frame_stack *interp_frame_stack_push(interp_frame_stack *stack, interp_frame *frame)
{
    stack = (interp_frame_stack *) buf_stack_push((buf_stack *) stack, frame, sizeof(*frame));
    interp_frame_stack_dump("after push", stack);
    return stack;
}

interp_frame_stack *interp_frame_stack_pop(interp_frame_stack *stack, interp_frame **removed_frame)
{
    stack = (interp_frame_stack *) buf_stack_pop((buf_stack *) stack, (void **) removed_frame);
    interp_frame_stack_dump("after pop", stack);
    return stack;
}

interp_frame *interp_frame_stack_index(interp_frame_stack *stack, size_t index)
{
    return (interp_frame *) buf_stack_index((buf_stack *) stack, index);
}

const interp_frame *interp_frame_stack_index_const(const interp_frame_stack *stack, size_t index)
{
    return (const interp_frame *) buf_stack_index_const((const buf_stack *) stack, index);
}
