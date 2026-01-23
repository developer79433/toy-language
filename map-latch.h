#ifndef TOY_MAP_LATCH_H
#define TOY_MAP_LATCH_H 1

#include "map-latch-types.h"

void map_latch_init(map_latch *latch, toy_bool stop_on_first);
generic_map_entry *map_latch_get_last_seen(map_latch *latch);

#endif /* TOY_MAP_LATCH_H */
