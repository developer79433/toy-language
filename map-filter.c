#include <assert.h>

#include "map-filter.h"
#include "map-visitor.h"

static enumeration_result generic_map_filter_visitor_visit_entry(map_filter *visitor, generic_map_entry *entry)
{
    if (visitor->filter(visitor->filter_cookie, entry)) {
        assert(
            (void *) visitor->visit ==
            (void *) ((map_visitor *) visitor)->visit
        );
        return visitor->visit(visitor, entry);
    }
    return CONTINUE_ENUMERATION;
}

/* TODO: Should this take a generic_map_filter_visitor or a generic_map_visitor? */
enumeration_result generic_map_filter_visitor_visit_map(map_filter *visitor, generic_map *map)
{
    /* FIXME: Never uses visitor parameter */
    assert(0);
    map_visitor filter_visitor = {
        .visit = (map_entry_visit_func) generic_map_filter_visitor_visit_entry
    };
    return map_visitor_visit(&filter_visitor, map);
}
