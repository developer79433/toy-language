#ifndef TOY_VAL_LIST_H
#define TOY_VAL_LIST_H 1

#include <stdio.h>
#include <stddef.h>

#include "toy-val-list-types.h"
#include "toy-num-types.h"

size_t val_list_len(const toy_val_list *list);
toy_val *val_list_index(toy_val_list *list, toy_num index);
void dump_val_list(FILE *f, toy_val_list *list);
toy_val_list *append_val_list(toy_val_list *orig, toy_val_list *new_item);
toy_val_list *append_val_list_own(toy_val_list *orig, toy_val *new_item);
toy_val_list *alloc_val_list(toy_val *first_elem);
toy_val_list *alloc_val_list_own(toy_val *first_elem);

#endif /* TOY_VAL_LIST_H */
