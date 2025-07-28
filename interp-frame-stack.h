#ifndef TOY_INTERP_FRAME_STACK_H
#define TOY_INTERP_FRAME_STACK_H 1

#include "iter-types.h"
#include "interp-frame-stack-types.h"

interp_frame *interp_frame_stack_payload(interp_frame_stack *stack);
const interp_frame *interp_frame_stack_payload_const(const interp_frame_stack *stack);
interp_frame_stack *interp_frame_stack_push(interp_frame_stack *stack, interp_frame_stack *other_stack);
interp_frame_stack *interp_frame_stack_pop(interp_frame_stack *stack);
void interp_frame_stack_free(interp_frame_stack *stack);
void interp_frame_stack_assert_valid(const interp_frame_stack *frame_list);
void interp_frame_stack_dump(FILE *f, const char *context, const interp_frame_stack *stack);

#endif /* TOY_INTERP_FRAME_STACK_H */
