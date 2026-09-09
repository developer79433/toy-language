#include <assert.h>

#include "generic-list.h"
#include "list-visitor.h"
#include "list-filter.h"
#include "log.h"

#if 0
#define DEBUG_LIST_FILTER
#endif

void list_filter_init(list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted)
{
    filter->visitor.visit_entry = (list_entry_visit_func) list_filter_visit_entry;
    filter->visitor.prev_item = NULL;
    filter->filter_func = filter_func;
    filter->filter_cookie = filter_cookie;
    filter->inverted = inverted;
    filter->last_match = NULL;
    filter->last_filter_res = TOY_TRUE;
}

item_callback_result list_filter_visit_entry(list_filter *filter, size_t index, generic_list *entry)
{
    assert((void *) filter == (void *) &filter->visitor);
    toy_bool filter_res = filter->filter_func(filter->filter_cookie, index, entry);
    if (filter->inverted) {
        filter_res = !filter_res;
    }
    filter->last_filter_res = filter_res;
    if (filter_res) {
        filter->last_match = entry;
    }
#ifdef DEBUG_LIST_FILTER
    log_debug_file(
        "filter %p %s, we are %sinverted, last match is now %p\n",
        filter,
        filter_res ? "succeeded" : "failed",
        filter->inverted ? "" : "not ",
        filter->last_match
    );
#endif /* DEBUG_LIST_FILTER */
    return CONTINUE_ENUMERATION;
}

toy_bool list_filter_last_result(const list_filter *filter)
{
    return filter->last_filter_res;
}

generic_list *list_filter_last_match(const list_filter *filter)
{
    return filter->last_match;
}

void const_list_filter_init(const_list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted)
{
    filter->visitor.visit_entry = (const_list_entry_visit_func) const_list_filter_visit_entry;
    filter->visitor.prev_item = NULL;
    filter->filter_func = filter_func;
    filter->filter_cookie = filter_cookie;
    filter->inverted = inverted;
    filter->last_match = NULL;
    filter->last_filter_res = TOY_TRUE;
}

item_callback_result const_list_filter_visit_entry(const_list_filter *filter, size_t index, const generic_list *entry)
{
    assert((void *) filter == (void *) &filter->visitor);
    toy_bool filter_res = filter->filter_func(filter->filter_cookie, index, entry);
    if (filter->inverted) {
        filter_res = !filter_res;
    }
    filter->last_filter_res = filter_res;
    if (filter_res) {
        filter->last_match = entry;
    }
#ifdef DEBUG_LIST_FILTER
    log_debug_file(
        "filter %p %s, we are %sinverted, last match is now %p\n",
        filter,
        filter_res ? "succeeded" : "failed",
        filter->inverted ? "" : "not ",
        filter->last_match
    );
#endif /* DEBUG_LIST_FILTER */
    return CONTINUE_ENUMERATION;
}

toy_bool const_list_filter_last_result(const const_list_filter *filter)
{
    return filter->last_filter_res;
}

const generic_list *const_list_filter_last_match(const const_list_filter *filter)
{
    return filter->last_match;
}
