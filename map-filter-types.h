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
    map_visitor *next_visitor;
};

struct const_map_filter_struct;
typedef struct const_map_filter_struct const_map_filter;
struct const_map_filter_struct {
    const_map_visitor visitor;
    generic_map_filter_func filter_func;
    void *filter_cookie;
    const_map_visitor *next_visitor;
};

#endif /* TOY_MAP_FILTER_TYPES_H */
