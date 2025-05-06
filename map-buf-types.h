#ifndef TOY_MAP_BUF_TYPES_H
#define TOY_MAP_BUF_TYPES_H

#include <inttypes.h>

#include "iter-types.h"
#include "str-types.h"
#include "map-buf-entry-list-types.h"

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

typedef struct map_buf_struct {
    size_t num_items;
    map_buf_entry_list *buckets[NUM_BUCKETS];
} map_buf;

/* TODO: Insert a 'size_t unused' argument for compatibility with list enumeration callbacks */
typedef item_callback_result (*map_buf_entry_callback)(void *cookie, map_buf_entry *entry);
typedef item_callback_result (*const_map_buf_entry_callback)(void *cookie, const map_buf_entry *entry);

#endif /* TOY_MAP_BUF_TYPES_H */
