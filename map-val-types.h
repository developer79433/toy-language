#ifndef TOY_MAP_VAL_TYPES_H
#define TOY_MAP_VAL_TYPES_H 1

#include <stddef.h>

#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"

struct toy_val_struct;
typedef struct toy_val_struct toy_val;

typedef struct map_val_entry_struct {
    toy_val value;
} map_val_entry;

typedef struct map_val_entry_list_struct {
    struct map_val_entry_list_struct *next;
    toy_str key;
    map_val_entry entry;
} map_val_entry_list;

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

/* TODO: Use generic list lib */
typedef struct map_val_struct {
    size_t num_items;
    map_val_entry_list *buckets[NUM_BUCKETS];
} map_val;

typedef item_callback_result (*map_val_entry_callback)(void *cookie, toy_str key, toy_val *value);
typedef item_callback_result (*const_map_val_entry_callback)(void *cookie, const toy_str key, const toy_val *value);

#endif /* TOY_MAP_VAL_TYPES_H */
