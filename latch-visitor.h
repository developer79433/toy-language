#ifndef TOY_LATCH_VISITOR_H
#define TOY_LATCH_VISITOR_H 1

#include "latch-visitor-types.h"

void latch_visitor_init(latch_visitor *latch, toy_bool stop_on_first);
item_callback_result latch_visitor_visit_entry(latch_visitor *latch, size_t index, generic_list *item);

#endif /* TOY_LATCH_VISITOR_H */
