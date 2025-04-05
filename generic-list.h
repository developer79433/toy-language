#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H 1

#include <stddef.h>

#include "generic-list-types.h"

size_t generic_list_len(const generic_list *list);
generic_list *generic_list_append(generic_list *orig_list, generic_list *new_list);

#endif /* GENERIC_LIST_H */
