#ifndef TOY_VAL_LIST_H
#define TOY_VAL_LIST_H 1

#include <stddef.h>

#include "bool-types.h"
#include "val-list-types.h"
#include "num-types.h"

size_t val_list_len(const toy_val_list *list);
toy_val *val_list_index(toy_val_list *list, size_t index);
const toy_val *val_list_index_const(const toy_val_list *list, size_t index);
void val_list_dump(const toy_val_list *list);
toy_val_list *val_list_concat(toy_val_list *orig, toy_val_list *new_item);
toy_val_list *val_list_append(toy_val_list *orig, const toy_val *new_item);
toy_val_list *val_list_alloc(const toy_val *first_elem);
toy_val *val_list_payload(toy_val_list *list);
const void *val_list_payload_const(const toy_val_list *list);
void val_list_set_payload(toy_val_list *list, const toy_val *value);
void val_list_assert_valid(const toy_val_list *val_list);
void val_list_free(toy_val_list *list);

#endif /* TOY_VAL_LIST_H */
