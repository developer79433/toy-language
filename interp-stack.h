#ifndef TOY_INTERP_STACK_H
#define TOY_INTERP_STACK_H 1

#include "interp-frame-types.h"
#include "interp-stack-types.h"

void interp_stack_assert_valid(const interp_stack *stack);
interp_frame *interp_stack_payload(interp_stack *stack);
void interp_stack_free(interp_stack *stack);
const interp_frame *interp_stack_payload_const(const interp_stack *stack);
void interp_stack_dump(const char *context, const interp_stack *stack);
size_t interp_stack_len(const interp_stack *stack);
/* TODO: Can these be unified? */
interp_stack *interp_stack_push_predef_func(interp_stack *stack, const toy_function *func_def, const toy_val_list *args);
interp_stack *interp_stack_push_user_func(interp_stack *stack, const toy_function *func_def, const toy_val_list *args);
interp_stack *interp_stack_push_block(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_push_if(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_push_loop(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_pop(interp_stack *stack);
interp_frame *interp_stack_index(interp_stack *stack, size_t index);
const interp_frame *interp_stack_index_const(const interp_stack *stack, size_t index);

#endif /* TOY_INTERP_STACK_H */
