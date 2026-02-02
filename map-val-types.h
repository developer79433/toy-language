#ifndef TOY_MAP_VAL_TYPES_H
#define TOY_MAP_VAL_TYPES_H 1

#include <stddef.h>

#include "map-val-entry-list-types.h"

typedef struct map_val_struct {
    size_t num_items;
    size_t num_buckets;
    map_val_entry_list **buckets;
} map_val;

#endif /* TOY_MAP_VAL_TYPES_H */
