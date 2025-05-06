#ifndef TOY_GENERIC_MAP_TYPES_H
#define TOY_GENERIC_MAP_TYPES_H 1

#include <stddef.h>

#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"
#include "buf-list-types.h"

/* TODO: Delete me */
typedef struct generic_map_payload_struct {
    unsigned int dummy_payload;
} generic_map_payload;

typedef struct generic_map_entry_struct {
    toy_str key;
    generic_map_payload payload;
} generic_map_entry;

typedef struct generic_map_entry_list_struct {
    struct generic_map_entry_list_struct *next;
    generic_map_entry entry;
} generic_map_entry_list;

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

typedef struct generic_map_struct {
    size_t num_items;
    toy_buf_list *buckets[NUM_BUCKETS];
} generic_map;

typedef item_callback_result (*generic_map_entry_callback)(void *cookie, generic_map_entry *entry);
typedef item_callback_result (*const_generic_map_entry_callback)(void *cookie, const generic_map_entry *entry);

#endif /* TOY_GENERIC_MAP_TYPES_H */
