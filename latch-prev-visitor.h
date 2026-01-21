#ifndef TOY_LATCH_PREV_VISITOR_H
#define TOY_LATCH_PREV_VISITOR_H 1

#include "latch-prev-visitor-types.h"

void latch_prev_visitor_init(latch_prev_visitor *latch_prev, generic_list *list, toy_bool stop_on_first);
item_callback_result latch_prev_visitor_visit_entry(latch_prev_visitor *latch_prev, size_t index, generic_list *item);

#endif /* TOY_LATCH_PREV_VISITOR_H */
