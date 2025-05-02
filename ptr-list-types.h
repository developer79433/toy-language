#ifndef TOY_PTR_LIST_TYPES_H
#define TOY_PTR_LIST_TYPES_H 1

#include "generic-list-types.h"

typedef struct toy_ptr_list_struct {
    struct toy_ptr_list_struct *next;
    void *ptr;
} toy_ptr_list;

typedef item_callback_result (*ptr_list_item_callback)(void *cookie, size_t index, toy_ptr_list *item);
typedef item_callback_result (*const_ptr_list_item_callback)(void *cookie, size_t index, const toy_ptr_list *item);

#endif /* TOY_PTR_LIST_TYPES_H */
