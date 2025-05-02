#ifndef TOY_VAL_LIST_TYPES_H
#define TOY_VAL_LIST_TYPES_H 1

#include <stddef.h>

#include "generic-list.h"

struct toy_val_struct;
typedef struct toy_val_struct toy_val;

typedef struct toy_val_list_struct {
    struct toy_val_list_struct *next;
    toy_val val;
} toy_val_list;

typedef item_callback_result (*val_list_item_callback)(void *cookie, size_t index, toy_val_list *val);
typedef item_callback_result (*const_val_list_item_callback)(void *cookie, size_t index, const toy_val_list *val);

#endif /* VAL_TOY_LIST_TYPES_H */
