#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H 1

#include <stddef.h>

#include "num-types.h"
#include "generic-list-types.h"

generic_list *generic_list_alloc_size(size_t value_size);
generic_list *generic_list_concat(generic_list *orig_list, generic_list *new_list);
enumeration_result generic_list_foreach(generic_list *list, generic_list_item_callback callback, void *cookie);
enumeration_result generic_list_foreach_const(const generic_list *list, const_generic_list_item_callback callback, void *cookie);
void generic_list_free(generic_list *list);
generic_list *generic_list_index(generic_list *list, size_t index);
size_t generic_list_len(const generic_list *list);
enumeration_result generic_list_find_all(generic_list *list, generic_list_filter_func filter, generic_list_item_callback callback, void *cookie);
generic_list *generic_list_find_first(generic_list *list, generic_list_filter_func filter, void *cookie);
generic_list *generic_list_last(generic_list *list);

#endif /* GENERIC_LIST_H */
