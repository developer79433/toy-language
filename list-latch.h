#ifndef TOY_LIST_LATCH_H
#define TOY_LIST_LATCH_H 1

#include "list-latch-types.h"

void list_latch_init(list_latch *latch, toy_bool stop_on_first);
item_callback_result list_latch_visit_entry(list_latch *latch, size_t index, generic_list *item);

#endif /* TOY_LIST_LATCH_H */
