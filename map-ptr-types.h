#ifndef TOY_MAP_PTR_TYPES_H
#define TOY_MAP_PTR_TYPES_H

#include "iter-types.h"
#include "str-types.h"
#include "map-ptr-entry-list-types.h"

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

typedef struct map_ptr_struct {
    size_t num_items;
    map_ptr_entry_list *buckets[NUM_BUCKETS];
} map_ptr;

typedef item_callback_result (*map_ptr_entry_callback)(void *cookie, map_ptr_entry *entry);
typedef item_callback_result (*const_map_ptr_entry_callback)(void *cookie, const map_ptr_entry *entry);

#endif /* TOY_MAP_PTR_TYPES_H */
