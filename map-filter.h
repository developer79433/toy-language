#ifndef TOY_MAP_FILTER_H
#define TOY_MAP_FILTER_H 1

#include "iter-types.h"
#include "map-filter-types.h"
#include "generic-map-types.h"

void map_filter_init(map_filter *filter, generic_map_filter_func filter_func, void *filter_cookie, toy_bool inverted, map_visitor *next_visitor);
enumeration_result map_filter_visit_map(map_filter *filter, generic_map *map);
void const_map_filter_init(const_map_filter *filter, generic_map_filter_func filter_func, void *filter_cookie, const_map_visitor *next_visitor);
enumeration_result const_map_filter_visit_map(const_map_filter *filter, const generic_map *map);

#endif /* TOY_MAP_FILTER_H */
