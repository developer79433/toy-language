#ifndef TOY_MAP_VAL_TYPES_H
#define TOY_MAP_VAL_TYPES_H 1

#include <stddef.h>

#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"
#include "map-val-entry-list-types.h"

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

typedef struct map_val_struct {
    size_t num_items;
    map_val_entry_list *buckets[NUM_BUCKETS];
} map_val;

#endif /* TOY_MAP_VAL_TYPES_H */
