#include <assert.h>

#include "map-filter.h"
#include "map-visitor.h"

static enumeration_result generic_map_filter_visit_entry(map_filter *filter, generic_map_entry *entry)
{
    if (filter->filter_func(filter->filter_cookie, entry)) {
        assert(
            (void *) filter->visitor.visit_entry ==
            (void *) ((map_visitor *) filter)->visit_entry
        );
        return filter->visitor.visit_entry(&filter->visitor, entry);
    }
    return CONTINUE_ENUMERATION;
}

/* TODO: Should this take a generic_map_filter_visitor or a generic_map_visitor? */
enumeration_result generic_map_filter_visit_map(map_filter *filter, generic_map *map)
{
    /* FIXME: Never uses filter parameter */
    assert(0);
    map_visitor filter_visitor = {
        .visit_entry = (map_entry_visit_func) generic_map_filter_visit_entry
    };
    return map_visitor_visit_map(&filter_visitor, map);
}
