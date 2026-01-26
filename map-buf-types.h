#ifndef TOY_MAP_BUF_TYPES_H
#define TOY_MAP_BUF_TYPES_H

#include <inttypes.h>

#include "iter-types.h"
#include "str-types.h"
#include "map-buf-entry-list-types.h"

typedef struct map_buf_struct {
    size_t num_items;
    size_t num_buckets;
    map_buf_entry_list **buckets;
} map_buf;

#endif /* TOY_MAP_BUF_TYPES_H */
