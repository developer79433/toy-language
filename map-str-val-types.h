#ifndef TOY_MAP_STR_VAL_TYPES_H
#define TOY_MAP_STR_VAL_TYPES_H 1

#include <stddef.h>

#include "map-str-val-entry-list-types.h"

typedef struct map_str_val_struct {
    size_t num_items;
    size_t num_buckets;
    map_str_val_entry_list **buckets;
} map_str_val;

#endif /* TOY_MAP_STR_VAL_TYPES_H */
