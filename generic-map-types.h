#ifndef TOY_GENERIC_MAP_TYPES_H
#define TOY_GENERIC_MAP_TYPES_H 1

#include <stddef.h>

#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"

typedef struct generic_map_entry_struct {
    toy_str key;
    toy_val value;
} generic_map_entry;

typedef struct generic_map_entry_list_struct {
    struct generic_map_entry_list_struct *next;
    generic_map_entry entry;
} generic_map_entry_list;

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

/* TODO: Use generic list lib */
typedef struct generic_map_struct {
    size_t num_items;
    generic_map_entry_list *buckets[NUM_BUCKETS];
} generic_map;

typedef item_callback_result (*generic_map_entry_callback)(void *cookie, toy_str key, void *value);
typedef item_callback_result (*const_generic_map_entry_callback)(void *cookie, const toy_str key, const void *value);

#endif /* TOY_GENERIC_MAP_TYPES_H */
