#include <assert.h>

#include "generic-list.h"
#include "list-visitor.h"
#include "list-filter.h"

void list_filter_init(list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted, list_visitor *next_visitor)
{
    filter->visitor.visit_entry = (list_entry_visit_func) list_filter_visit_entry;
    filter->filter_func = filter_func;
    filter->filter_cookie = filter_cookie;
    filter->inverted = inverted;
    filter->next_visitor = next_visitor;
}

enumeration_result list_filter_visit_list(list_filter *filter, generic_list *list)
{
    return list_visitor_visit_list(&filter->visitor, list);
}

item_callback_result list_filter_visit_entry(list_filter *filter, size_t index, generic_list *entry)
{
    assert((void *) filter == (void *) &filter->visitor);
    toy_bool filter_res = filter->filter_func(filter->filter_cookie, index, entry);
    if (filter->inverted) {
        filter_res = !filter_res;
    }
    if (filter_res) {
        list_visitor *next = filter->next_visitor;
        assert(next);
        return list_visitor_visit_entry(next, index, entry);
    }
    return CONTINUE_ENUMERATION;
}

void const_list_filter_init(const_list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted, const_list_visitor *next_visitor)
{
    filter->visitor.visit_entry = (const_list_entry_visit_func) const_list_filter_visit_entry;
    filter->filter_func = filter_func;
    filter->filter_cookie = filter_cookie;
    filter->inverted = inverted;
    filter->next_visitor = next_visitor;
}

enumeration_result const_list_filter_visit_list(const_list_filter *filter, const generic_list *list)
{
    return const_list_visitor_visit_list(&filter->visitor, list);
}

item_callback_result const_list_filter_visit_entry(const_list_filter *filter, size_t index, const generic_list *entry)
{
    assert((void *) filter == (void *) &filter->visitor);
    toy_bool filter_res = filter->filter_func(filter->filter_cookie, index, entry);
    if (filter->inverted) {
        filter_res = !filter_res;
    }
    if (filter_res) {
        const_list_visitor *next = filter->next_visitor;
        assert(next);
        return const_list_visitor_visit_entry(next, index, entry);
    }
    return CONTINUE_ENUMERATION;
}
