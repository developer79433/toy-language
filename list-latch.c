#include "list-latch.h"

void list_latch_init(list_latch *latch, toy_bool stop_on_first)
{
    latch->visitor.visit_list = NULL;
    latch->visitor.visit_entry = (list_entry_visit_func) list_latch_visit_entry;
    latch->last_seen_item = NULL;
    latch->stop_on_first = stop_on_first;
}

item_callback_result list_latch_visit_entry(list_latch *latch, size_t index, generic_list *item)
{
    latch->last_seen_item = item;
    return latch->stop_on_first ? STOP_ENUMERATION : CONTINUE_ENUMERATION;
}
