#ifndef TOY_MAP_VISITOR_H
#define TOY_MAP_VISITOR_H 1

#include "generic-map-types.h"
#include "map-visitor-types.h"
#include "iter-types.h"

enumeration_result map_visitor_visit_map(map_visitor *visitor, generic_map *map);
item_callback_result map_visitor_visit_entry_default(map_visitor *visitor, generic_map_entry *entry);
item_callback_result map_visitor_visit_entry(map_visitor *visitor, generic_map_entry *entry);

enumeration_result const_map_visitor_visit_map(const_map_visitor *visitor, const generic_map *map);
item_callback_result const_map_visitor_visit_entry_default(const_map_visitor *visitor, const generic_map_entry *entry);
item_callback_result const_map_visitor_visit_entry(const_map_visitor *visitor, const generic_map_entry *entry);

#endif /* TOY_MAP_VISITOR_H */
