#include "latch-prev-visitor.h"
#include "latch-visitor.h"

void latch_prev_visitor_init(latch_prev_visitor *latch_prev, generic_list *list, toy_bool stop_on_first)
{
    latch_visitor_init(&latch_prev->latch, stop_on_first);
    latch_prev->latch.base.visit_entry = (list_entry_visit_func) latch_prev_visitor_visit_entry;
    /* FIXME: Should this point to the list initially? */
    latch_prev->prev = NULL;
}

item_callback_result latch_prev_visitor_visit_entry(latch_prev_visitor *latch_prev, size_t index, generic_list *item)
{
    item_callback_result res = latch_visitor_visit_entry(&latch_prev->latch, index, item);
    latch_prev->prev = item;
    return res;
}
