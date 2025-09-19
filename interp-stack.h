#ifndef TOY_INTERP_STACK_H
#define TOY_INTERP_STACK_H 1

#include "interp-frame-types.h"
#include "interp-stack-types.h"

interp_frame *interp_stack_payload(interp_stack *stack);
void interp_stack_free(interp_stack *stack);
const interp_frame *interp_stack_payload_const(const interp_stack *stack);
/* FIXME: Use log functions instead of taking a FILE * argument */
void interp_stack_dump(const char *context, const interp_stack *stack);
/* TODO: Can these be unified? */
interp_stack *interp_stack_push_predef_func(interp_stack *stack, const toy_function *func_def, const toy_val_list *args);
interp_stack *interp_stack_push_user_func(interp_stack *stack, const toy_function *func_def, const toy_val_list *args);
interp_stack *interp_stack_push_block(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_push_if(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_push_loop(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_pop(interp_stack *stack);
toy_val *interp_stack_get_variable(interp_stack *stack, size_t frames_up, size_t var_index);
toy_val *interp_stack_get_func_param(interp_stack *stack, size_t frames_up, size_t param_index);
interp_frame *interp_stack_index(interp_stack *stack, size_t index);
const interp_frame *interp_stack_index_const(const interp_stack *stack, size_t index);

#endif /* TOY_INTERP_STACK_H */
