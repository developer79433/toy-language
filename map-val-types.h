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

typedef item_callback_result (*map_val_entry_callback)(void *cookie, toy_str key, toy_val *value);
typedef item_callback_result (*const_map_val_entry_callback)(void *cookie, const toy_str key, const toy_val *value);

#endif /* TOY_MAP_VAL_TYPES_H */
