#ifndef TOY_VAL_LIST_H
#define TOY_VAL_LIST_H 1

#include <stdio.h>
#include <stddef.h>

#include "val-list-types.h"
#include "num-types.h"

size_t val_list_len(const toy_val_list *list);
toy_val *val_list_index(toy_val_list *list, size_t index);
void val_list_dump(FILE *f, toy_val_list *list);
toy_val_list *val_list_concat(toy_val_list *orig, toy_val_list *new_item);
toy_val_list *val_list_append(toy_val_list *orig, toy_val *new_item);
toy_val_list *val_list_alloc(toy_val *first_elem);
enumeration_result val_list_foreach(toy_val_list *list, val_list_item_callback callback, void *cookie);
enumeration_result val_list_foreach_const(const toy_val_list *list, const_val_list_item_callback callback, void *cookie);
void val_list_assert_valid(const toy_val_list *val_list);
void val_list_free(toy_val_list *list);

#endif /* TOY_VAL_LIST_H */
