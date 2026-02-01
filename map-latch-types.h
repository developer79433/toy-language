#ifndef TOY_MAP_LATCH_TYPES_H
#define TOY_MAP_LATCH_TYPES_H 1

#include "generic-map-types.h"
#include "map-visitor-types.h"

typedef struct map_latch_struct {
    map_visitor visitor;
    generic_map_entry *last_seen_entry;
} map_latch;

typedef struct const_map_latch_struct {
    const_map_visitor visitor;
    const generic_map_entry *last_seen_entry;
} const_map_latch;

#endif /* TOY_MAP_LATCH_TYPES_H */
