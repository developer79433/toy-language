#include <assert.h>

#include "map-filter.h"
#include "map-visitor.h"

static enumeration_result map_filter_visit_entry(map_filter *filter, generic_map_entry *entry)
{
    toy_bool filter_res = filter->filter_func(filter->filter_cookie, entry);
    if (filter->inverted) {
        filter_res = !filter_res;
    }
    if (filter_res) {
        assert(
            (void *) filter->visitor.visit_entry ==
            (void *) ((map_visitor *) filter)->visit_entry
        );
        map_visitor *next = filter->next_visitor;
        assert(next);
        return next->visit_entry(next, entry);
    }
    return CONTINUE_ENUMERATION;
}

void map_filter_init(map_filter *filter, generic_map_filter_func filter_func, void *filter_cookie, toy_bool inverted, map_visitor *next_visitor)
{
    filter->visitor.visit_entry = (map_entry_visit_func) map_filter_visit_entry;
    filter->filter_func = filter_func;
    filter->filter_cookie = filter_cookie;
    filter->inverted = inverted;
    filter->next_visitor = next_visitor;
}

enumeration_result map_filter_visit_map(map_filter *filter, generic_map *map)
{
    return map_visitor_visit_map((map_visitor *) filter, map);
}

static enumeration_result const_map_filter_visit_entry(const_map_filter *filter, const generic_map_entry *entry)
{
    toy_bool filter_res = filter->filter_func(filter->filter_cookie, entry);
    if (filter->inverted) {
        filter_res = !filter_res;
    }
    if (filter_res) {
        assert(
            (void *) filter->visitor.visit_entry ==
            (void *) ((map_visitor *) filter)->visit_entry
        );
        const_map_visitor *next = filter->next_visitor;
        assert(next);
        return next->visit_entry(next, entry);
    }
    return CONTINUE_ENUMERATION;
}

void const_map_filter_init(const_map_filter *filter, generic_map_filter_func filter_func, void *filter_cookie, toy_bool inverted, const_map_visitor *next_visitor)
{
    filter->visitor.visit_entry = (const_map_entry_visit_func) const_map_filter_visit_entry;
    filter->filter_func = filter_func;
    filter->filter_cookie = filter_cookie;
    filter->inverted = inverted;
    filter->next_visitor = next_visitor;
}

enumeration_result const_map_filter_visit_map(const_map_filter *filter, const generic_map *map)
{
    return const_map_visitor_visit_map((const_map_visitor *) filter, map);
}
