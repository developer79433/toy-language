#ifndef TOY_MAP_SIZE_T_TYPES_H
#define TOY_MAP_SIZE_T_TYPES_H 1

#include "map-size-t-entry-list-types.h"

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

typedef struct map_size_t_struct {
    size_t num_items;
    map_size_t_entry_list *buckets[NUM_BUCKETS];
} map_size_t;

typedef item_callback_result (*map_size_t_entry_callback)(void *cookie, map_size_t_entry *entry);
typedef item_callback_result (*const_map_size_t_entry_callback)(void *cookie, const map_size_t_entry *entry);

#endif /* TOY_MAP_SIZE_T_TYPES_H */
