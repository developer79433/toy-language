#ifndef TOY_BUF_STACK_TYPES_H
#define TOY_BUF_STACK_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"

struct buf_stack_struct;
typedef struct buf_stack_struct buf_stack;

typedef item_callback_result(*buf_stack_item_callback)(void *cookie, size_t index, buf_stack *item);
typedef item_callback_result(*const_buf_stack_item_callback)(void *cookie, size_t index, const buf_stack *item);

#endif /* TOY_BUF_STACK_TYPES_H */
