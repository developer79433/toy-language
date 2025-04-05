#ifndef TOY_STR_LIST_H
#define TOY_STR_LIST_H 1

#include <stdio.h>
#include <stddef.h>

#include "toy-str-list-types.h"

size_t str_list_len(const toy_str_list *list);
void dump_str_list(FILE *f, const toy_str_list *list);
toy_str_list *alloc_str_list(const char * str);
toy_str_list *append_str_list(toy_str_list *orig, toy_str_list *new);

#endif /* TOY_STR_LIST_H */
