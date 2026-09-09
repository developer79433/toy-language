#ifndef TOY_MAP_BUF_PTR_TYPES_H
#define TOY_MAP_BUF_PTR_TYPES_H

#include "iter-types.h"
#include "map-buf-ptr-entry-list-types.h"

typedef struct map_buf_ptr_struct {
    size_t num_items;
    size_t num_buckets;
    map_buf_ptr_entry_list **buckets;
} map_buf_ptr;

#endif /* TOY_MAP_BUF_PTR_TYPES_H */
