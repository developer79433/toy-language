#ifndef TOY_INTERP_STACK_H
#define TOY_INTERP_STACK_H 1

/* FIXME: Remove includes of stdio */
#include <stdio.h>

#include "interp-frame-types.h"
#include "interp-stack-types.h"

interp_frame *interp_stack_payload(interp_stack *stack);
void interp_stack_free(interp_stack *stack);
const interp_frame *interp_stack_payload_const(const interp_stack *stack);
/* FIXME: Use log functions instead of taking a FILE * argument */
void interp_stack_dump(FILE *f, const char *context, const interp_stack *stack);
/* TODO: Can these be unified? */
interp_stack *interp_stack_push_predef_func(interp_stack *stack, const toy_function *func_def, const toy_val_list *args);
interp_stack *interp_stack_push_user_func(interp_stack *stack, const toy_function *func_def, const toy_val_list *args);
interp_stack *interp_stack_push_block(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_push_if(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_push_loop(interp_stack *stack, const toy_block *block);
interp_stack *interp_stack_pop(interp_stack *stack);

#endif /* TOY_INTERP_STACK_H */
