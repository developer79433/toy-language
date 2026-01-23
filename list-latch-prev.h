#ifndef TOY_LIST_LATCH_PREV_H
#define TOY_LIST_LATCH_PREV_H 1

#include "list-latch-prev-types.h"

void list_latch_prev_init(list_latch_prev *latch_prev, generic_list *list, toy_bool stop_on_first);
item_callback_result list_latch_prev_visit_entry(list_latch_prev *latch_prev, size_t index, generic_list *item);

#endif /* TOY_LIST_LATCH_PREV_H */
