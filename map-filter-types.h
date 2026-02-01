#ifndef TOY_MAP_FILTER_TYPES_H
#define TOY_MAP_FILTER_TYPES_H 1

#include "generic-map-types.h"
#include "map-visitor-types.h"

struct map_filter_struct;
typedef struct map_filter_struct map_filter;
struct map_filter_struct {
    map_visitor visitor;
    generic_map_filter_func filter_func;
    void *filter_cookie;
    toy_bool inverted;
    generic_map_entry *last_match;
};

struct const_map_filter_struct;
typedef struct const_map_filter_struct const_map_filter;
struct const_map_filter_struct {
    const_map_visitor visitor;
    generic_map_filter_func filter_func;
    void *filter_cookie;
    toy_bool inverted;
    const generic_map_entry *last_match;
};

#endif /* TOY_MAP_FILTER_TYPES_H */
