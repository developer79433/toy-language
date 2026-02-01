#ifndef TOY_MAP_LATCH_H
#define TOY_MAP_LATCH_H 1

#include "map-latch-types.h"

void map_latch_init(map_latch *latch);
item_callback_result map_latch_visit_entry(map_latch *latch, generic_map_entry *entry);
generic_map_entry *map_latch_get_last_seen(map_latch *latch);
void const_map_latch_init(const_map_latch *latch);
item_callback_result const_map_latch_visit_entry(const_map_latch *latch, const generic_map_entry *entry);
const generic_map_entry *const_map_latch_get_last_seen(const_map_latch *latch);

#endif /* TOY_MAP_LATCH_H */
