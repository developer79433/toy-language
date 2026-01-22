#ifndef TOY_MAP_FILTER_TYPES_H
#define TOY_MAP_FILTER_TYPES_H 1

#include "generic-map-types.h"

struct map_filter_struct;
typedef struct map_filter_struct map_filter;
typedef item_callback_result (*map_filter_entry_visit_func)(map_filter *visitor, generic_map_entry *entry);
struct map_filter_struct {
    map_filter_entry_visit_func visit;
    generic_map_filter_func filter;
    void *filter_cookie;
};

#endif /* TOY_MAP_FILTER_TYPES_H */
