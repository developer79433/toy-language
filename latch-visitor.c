#include "latch-visitor.h"

void latch_visitor_init(latch_visitor *latch, toy_bool stop_on_first)
{
    latch->base.visit_list = NULL;
    latch->base.visit_entry = (list_entry_visit_func) latch_visitor_visit_entry;
    latch->last_seen_item = NULL;
    latch->stop_on_first = stop_on_first;
}

item_callback_result latch_visitor_visit_entry(latch_visitor *latch, size_t index, generic_list *item)
{
    latch->last_seen_item = item;
    return latch->stop_on_first ? STOP_ENUMERATION : CONTINUE_ENUMERATION;
}
