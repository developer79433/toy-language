#ifndef ptr_stack_H
#define ptr_stack_H 1

#include "iter-types.h"
#include "ptr-stack-types.h"

void ptr_stack_init(ptr_stack *stack);
ptr_stack *ptr_stack_alloc(void *ptr);
size_t ptr_stack_len(const ptr_stack *stack);
void ptr_stack_free(ptr_stack *stack);
void *ptr_stack_payload(ptr_stack *stack);
const void *ptr_stack_payload_const(const ptr_stack *stack);
ptr_stack *ptr_stack_push(ptr_stack *stack, void *ptr);
ptr_stack *ptr_stack_pop(ptr_stack *stack, void **removed_ptr);
enumeration_result ptr_stack_foreach(ptr_stack *stack, ptr_stack_item_callback callback, void *cookie);
enumeration_result ptr_stack_foreach_const(const ptr_stack *stack, const_ptr_stack_item_callback callback, void *cookie);

#endif /* ptr_stack_H */
