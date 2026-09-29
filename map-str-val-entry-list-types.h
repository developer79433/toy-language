#ifndef TOY_MAP_STR_VAL_ENTRY_LIST_TYPES_H
#define TOY_MAP_STR_VAL_ENTRY_LIST_TYPES_H 1

#include <stddef.h>

#include "val-types.h"

typedef struct map_str_val_entry_struct {
    void *key;
    size_t key_len;
    toy_val value;
} map_str_val_entry;

typedef struct map_str_val_entry_list_struct {
    struct map_str_val_entry_list_struct *next;
    map_str_val_entry entry;
} map_str_val_entry_list;

#endif /* TOY_MAP_STR_VAL_ENTRY_LIST_TYPES_H */
