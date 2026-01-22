#ifndef TOY_MAP_VISITOR_H
#define TOY_MAP_VISITOR_H 1

#include "generic-map-types.h"
#include "map-visitor-types.h"
#include "iter-types.h"

enumeration_result map_visitor_visit(map_visitor *visitor, generic_map *map);
enumeration_result const_map_visitor_visit(const_map_visitor *visitor, const generic_map *map);

#endif /* TOY_MAP_VISITOR_H */
