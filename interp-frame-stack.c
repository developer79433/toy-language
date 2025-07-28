#include <assert.h>
#include <stdio.h>

#include "interp-frame-stack.h"
#include "interp-frame.h"
#include "mymalloc.h"
#include "symbol-table.h"
#include "interp.h"
#include "debug.h"
#include "buf-stack.h"

/* TODO: Use buf_stack */

interp_frame *interp_frame_stack_payload(interp_frame_stack *stack)
{
    return &stack->frame;
}

const interp_frame *interp_frame_stack_payload_const(const interp_frame_stack *stack)
{
    return &stack->frame;
}

void interp_frame_stack_free(interp_frame_stack *stack)
{
    while (stack) {
        interp_frame_stack *prev = stack->prev;
        interp_frame_free(&stack->frame);
        stack = prev;
    }
}

void interp_frame_stack_assert_valid(const interp_frame_stack *frame_list)
{
    const interp_frame *frame = interp_frame_stack_payload_const(frame_list);
    interp_frame_assert_valid(frame);
}

void interp_frame_stack_dump(FILE *f, const char *context, const interp_frame_stack *stack)
{
    fprintf(stderr, "STACK %s:\n", context);
    size_t frame_num = 0;
    /* TODO: Use buf_stack_foreach */
    for (; stack; stack = stack->prev, frame_num++) {
        fprintf(stderr, "  Frame %02lu: ", (unsigned long) frame_num);
        const interp_frame *frame = interp_frame_stack_payload_const(stack);
        interp_frame_dump(f, frame);
        fprintf(stderr, "\n");
    }
}

interp_frame_stack *interp_frame_stack_push(interp_frame_stack *stack, interp_frame_stack *other_stack)
{
    other_stack->prev = stack;
    return stack;
}

interp_frame_stack *interp_frame_stack_pop(interp_frame_stack *stack)
{
    interp_frame_stack *prev = stack->prev;
    interp_frame_stack_dump(stderr, "after pop", stack);
    return prev;
}
