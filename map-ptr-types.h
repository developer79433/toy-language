#ifndef TOY_MAP_PTR_TYPES_H
#define TOY_MAP_PTR_TYPES_H

#include "iter-types.h"
#include "str-types.h"
#include "map-ptr-entry-list-types.h"

typedef struct map_ptr_struct {
    size_t num_items;
    size_t num_buckets;
    map_ptr_entry_list **buckets;
} map_ptr;

#endif /* TOY_MAP_PTR_TYPES_H */
