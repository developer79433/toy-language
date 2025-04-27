#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H 1

#include <stddef.h>

#include "num-types.h"
#include "generic-list-types.h"

generic_list *generic_list_alloc_ref(void *first_elem);
generic_list *generic_list_alloc_own(void *first_elem, size_t value_size);
size_t generic_list_len(const generic_list *list);
generic_list *generic_list_concat(generic_list *orig_list, generic_list *new_list);
generic_list *generic_list_prepend_ref(generic_list *list, void *new_payload);
generic_list *generic_list_prepend_own(generic_list *list, void *new_payload, size_t payload_size);
generic_list *generic_list_append_ref(generic_list *list, void *new_payload);
generic_list *generic_list_append_own(generic_list *list, void *new_payload, size_t payload_size);
void *generic_list_index(generic_list *list, toy_num index);
void generic_list_foreach(void *list, list_item_callback callback, void *cookie);
void generic_list_foreach_const(const void *list, const_list_item_callback callback, void *cookie);

#endif /* GENERIC_LIST_H */
