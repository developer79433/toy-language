#ifndef TOY_MAP_BUF_BUF_TYPES_H
#define TOY_MAP_BUF_BUF_TYPES_H

#include <stddef.h>

#include "iter-types.h"
#include "str-types.h"
#include "map-buf-buf-entry-list-types.h"

typedef struct map_buf_buf_struct {
    size_t num_items;
    size_t num_buckets;
    map_buf_buf_entry_list **buckets;
} map_buf_buf;

#endif /* TOY_MAP_BUF_BUF_TYPES_H */
