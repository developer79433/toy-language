#ifndef TOY_STR_LIST_H
#define TOY_STR_LIST_H 1

#include <stdio.h>
#include <stddef.h>

#include "str-list-types.h"

toy_str_list *str_list_alloc(const char * str);
toy_str_list *str_list_append(toy_str_list *list, toy_str new_item);
toy_str_list *str_list_concat(toy_str_list *orig, toy_str_list *new);
void str_list_free(toy_str_list *str_list);
void str_list_dump(FILE *f, const toy_str_list *list);
toy_str str_list_index(toy_str_list *list, size_t index);
size_t str_list_len(const toy_str_list *list);
toy_str str_list_payload(toy_str_list *list);
const toy_str str_list_payload_const(const toy_str_list *list);
list_iter_result str_list_foreach(toy_str_list *list, toy_str_list_item_callback callback, void *cookie);
list_iter_result str_list_foreach_const(const toy_str_list *list, const_toy_str_list_item_callback callback, void *cookie);

#endif /* TOY_STR_LIST_H */
