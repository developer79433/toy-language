#ifndef TOY_MAP_FILTER_H
#define TOY_MAP_FILTER_H 1

#include "iter-types.h"
#include "map-filter-types.h"
#include "generic-map-types.h"

enumeration_result generic_map_filter(map_filter *visitor, generic_map *map);

#endif /* TOY_MAP_FILTER_H */
