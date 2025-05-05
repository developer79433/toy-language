#ifndef TOY_MAP_VAL_ENTRY_LIST_TYPES_H
#define TOY_MAP_VAL_ENTRY_LIST_TYPES_H 1

#include "val-types.h"

typedef struct map_val_payload_struct {
    toy_val value;
} map_val_payload;

typedef struct map_val_entry_struct {
    toy_str key;
    map_val_payload payload;
} map_val_entry;

typedef struct map_val_entry_list_struct {
    struct map_val_entry_list_struct *next;
    map_val_entry entry;
} map_val_entry_list;

#endif /* TOY_MAP_VAL_ENTRY_LIST_TYPES_H */
