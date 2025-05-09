#ifndef TOY_VAL_LIST_H
#define TOY_VAL_LIST_H 1

#include <stdio.h>
#include <stddef.h>

#include "val-list-types.h"
#include "num-types.h"

size_t val_list_len(const toy_val_list *list);
/* TODO: Provide a const accessor */
toy_val *val_list_index(toy_val_list *list, size_t index);
void val_list_dump(FILE *f, const toy_val_list *list);
toy_val_list *val_list_concat(toy_val_list *orig, toy_val_list *new_item);
toy_val_list *val_list_append(toy_val_list *orig, const toy_val *new_item);
toy_val_list *val_list_alloc(const toy_val *first_elem);
toy_val *val_list_payload(toy_val_list *list);
const void *val_list_payload_const(const toy_val_list *list);
void val_list_set_payload(toy_val_list *list, const toy_val *value);
enumeration_result val_list_foreach(toy_val_list *list, toy_val_list_item_callback callback, void *cookie);
enumeration_result val_list_foreach_const(const toy_val_list *list, const_toy_val_list_item_callback callback, void *cookie);
enumeration_result val_list_find_all(toy_val_list *list, toy_val_list_filter_func filter, void *filter_cookie, toy_val_list_item_callback callback, void *cookie);
enumeration_result val_list_find_all_const(const toy_val_list *list, toy_val_list_filter_func filter, void *filter_cookie, const_toy_val_list_item_callback callback, void *cookie);
void val_list_assert_valid(const toy_val_list *val_list);
void val_list_free(toy_val_list *list);

#endif /* TOY_VAL_LIST_H */
