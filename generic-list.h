#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H 1

#include <stddef.h>

#include "num-types.h"
#include "generic-list-types.h"

generic_list *generic_list_alloc_size(size_t value_size);
generic_list *generic_list_concat(generic_list *orig_list, generic_list *new_list);
list_iter_result generic_list_foreach(generic_list *list, generic_list_item_callback callback, void *cookie);
list_iter_result generic_list_foreach_const(const generic_list *list, const_generic_list_item_callback callback, void *cookie);
void generic_list_free(generic_list *list);
list_iter_result generic_list_index(generic_list *list, size_t index, generic_list_item_callback callback, void *cookie);
size_t generic_list_len(const generic_list *list);

#endif /* GENERIC_LIST_H */
