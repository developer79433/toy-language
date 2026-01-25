#ifndef TOY_INTERP_FRAME_STACK_H
#define TOY_INTERP_FRAME_STACK_H 1

#include "iter-types.h"
#include "interp-frame-stack-types.h"

interp_frame *interp_frame_stack_payload(interp_frame_stack *stack);
size_t interp_frame_stack_len(const interp_frame_stack *stack);
const interp_frame *interp_frame_stack_payload_const(const interp_frame_stack *stack);
interp_frame_stack *interp_frame_stack_push(interp_frame_stack *stack, interp_frame *frame);
interp_frame_stack *interp_frame_stack_pop(interp_frame_stack *stack, interp_frame **removed_frame);
void interp_frame_stack_free(interp_frame_stack *stack);
void interp_frame_stack_assert_valid(const interp_frame_stack *frame_list);
void interp_frame_stack_dump(const char *context, const interp_frame_stack *stack);
interp_frame *interp_frame_stack_index(interp_frame_stack *stack, size_t index);
const interp_frame *interp_frame_stack_index_const(const interp_frame_stack *stack, size_t index);

#endif /* TOY_INTERP_FRAME_STACK_H */
