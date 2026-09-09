#include <assert.h>

#include "rev-list-visitor.h"
#include "generic-list.h"
#include "list-visitor.h"

void rev_list_visitor_init(rev_list_visitor *rev_list_vis, list_entry_visit_func visit_func)
{
    rev_list_vis->index = 0;
    rev_list_vis->list_vis.visit_entry = visit_func;
    rev_list_vis->list_vis.prev_item = NULL;
}

enumeration_result rev_list_visitor_visit_list(rev_list_visitor *rev_list_vis, generic_list *list)
{
    if (!list) {
        return ENUMERATION_COMPLETE;
    }
    generic_list *next = list_next(list);
    rev_list_vis->index++;
    enumeration_result enum_res = rev_list_visitor_visit_list(rev_list_vis, next);
    if (ENUMERATION_INTERRUPTED == enum_res) {
        return enum_res;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    rev_list_vis->index--;
    item_callback_result item_res = list_visitor_visit_entry((list_visitor *) rev_list_vis, rev_list_vis->index, list);
    if (STOP_ENUMERATION == item_res) {
        return ENUMERATION_INTERRUPTED;
    }
    assert(CONTINUE_ENUMERATION == item_res);
    return ENUMERATION_COMPLETE;
}

void const_rev_list_visitor_init(const_rev_list_visitor *rev_list_vis, const_list_entry_visit_func visit_func)
{
    rev_list_vis->index = 0;
    rev_list_vis->list_vis.visit_entry = visit_func;
    rev_list_vis->list_vis.prev_item = NULL;
}

enumeration_result const_rev_list_visitor_visit_list(const_rev_list_visitor *rev_list_vis, const generic_list *list)
{
    if (!list) {
        return ENUMERATION_COMPLETE;
    }
    const generic_list *next = list_next_const(list);
    rev_list_vis->index++;
    enumeration_result enum_res = const_rev_list_visitor_visit_list(rev_list_vis, next);
    if (ENUMERATION_INTERRUPTED == enum_res) {
        return enum_res;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    rev_list_vis->index--;
    item_callback_result item_res = const_list_visitor_visit_entry((const_list_visitor *) rev_list_vis, rev_list_vis->index, list);
    if (STOP_ENUMERATION == item_res) {
        return ENUMERATION_INTERRUPTED;
    }
    assert(CONTINUE_ENUMERATION == item_res);
    return ENUMERATION_COMPLETE;
}
