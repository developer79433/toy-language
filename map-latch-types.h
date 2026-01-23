#ifndef TOY_MAP_LATCH_TYPES_H
#define TOY_MAP_LATCH_TYPES_H 1

#include "generic-map-types.h"
#include "map-visitor-types.h"

typedef struct map_latch_struct {
    map_visitor visitor;
    generic_map_entry *last_seen_entry;
    toy_bool stop_on_first;
} map_latch;

#endif /* TOY_MAP_LATCH_TYPES_H */
