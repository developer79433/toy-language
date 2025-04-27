#ifndef TOY_STR_LIST_H
#define TOY_STR_LIST_H 1

#include <stdio.h>
#include <stddef.h>

#include "str-list-types.h"

size_t str_list_len(const toy_str_list *list);
void str_list_dump(FILE *f, const toy_str_list *list);
toy_str_list *str_list_alloc_ref(const char * str);
toy_str_list *str_list_alloc_own(const char * str);
toy_str_list *str_list_concat(toy_str_list *orig, toy_str_list *new);
toy_str_list *str_list_append_ref(toy_str_list *list, toy_str new_item);
toy_str_list *str_list_append_own(toy_str_list *list, toy_str new_item);

#endif /* TOY_STR_LIST_H */
