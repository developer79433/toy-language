#ifndef TOY_STR_LIST_TYPES_H
#define TOY_STR_LIST_TYPES_H 1

#include <stddef.h>

#include "str-types.h"
#include "generic-list-types.h"

typedef struct toy_str_list_struct {
    struct toy_str_list_struct *next;
    toy_str str;
} toy_str_list;

typedef listitem_callback_result (*toy_str_list_item_callback)(void *cookie, size_t index, toy_str_list item);
typedef listitem_callback_result (*const_toy_str_list_item_callback)(void *cookie, size_t index, const toy_str_list *item);

#endif /* TOY_STR_LIST_TYPES_H */
