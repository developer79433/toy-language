#ifndef TOY_PTR_STACK_TYPES_H
#define TOY_PTR_STACK_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"

struct ptr_stack_struct;
typedef struct ptr_stack_struct ptr_stack;

typedef item_callback_result(*ptr_stack_item_callback)(void *cookie, size_t index, ptr_stack *item);
typedef item_callback_result(*const_ptr_stack_item_callback)(void *cookie, size_t index, const ptr_stack *item);

#endif /* TOY_PTR_STACK_TYPES_H */
