#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H 1

#include <stddef.h>

#include "toy-num-types.h"
#include "generic-list-types.h"

size_t generic_list_len(const generic_list *list);
generic_list *generic_list_append(generic_list *orig_list, generic_list *new_list);
void *generic_list_index(generic_list *list, toy_num index);
void generic_list_foreach(void *list, list_item_callback callback, void *cookie);
void generic_list_foreach_const(const void *list, const_list_item_callback callback, void *cookie);

#endif /* GENERIC_LIST_H */
