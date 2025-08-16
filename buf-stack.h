#ifndef TOY_BUF_STACK_H
#define TOY_BUF_STACK_H 1

#include <stddef.h>

#include "iter-types.h"
#include "buf-stack-types.h"

void buf_stack_init(buf_stack *stack);
buf_stack *buf_stack_alloc(void *buf, size_t size);
size_t buf_stack_len(const buf_stack *stack);
void buf_stack_dump(const buf_stack *stack);
void buf_stack_free(buf_stack *stack);
void *buf_stack_payload(buf_stack *stack);
const void *buf_stack_payload_const(const buf_stack *stack);
buf_stack *buf_stack_push(buf_stack *stack, void *buf, size_t size);
buf_stack *buf_stack_pop(buf_stack *stack, void **removed_buf);
enumeration_result buf_stack_foreach(buf_stack *stack, buf_stack_item_callback callback, void *cookie);
enumeration_result buf_stack_foreach_const(const buf_stack *stack, const_buf_stack_item_callback callback, void *cookie);
buf_stack *buf_stack_index(buf_stack *stack, size_t index);
const buf_stack *buf_stack_index_const(const buf_stack *stack, size_t index);

#endif /* TOY_BUF_STACK_H */
