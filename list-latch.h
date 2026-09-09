#ifndef TOY_LIST_LATCH_H
#define TOY_LIST_LATCH_H 1

#include "list-latch-types.h"

/* TODO: Now that list iterators remember the last-seen entry, do we still need this? */
void list_latch_init(list_latch *latch);
item_callback_result list_latch_visit_entry(list_latch *latch, size_t index, generic_list *item);
generic_list *list_latch_get_last_seen(list_latch *latch);
void const_list_latch_init(const_list_latch *latch);
item_callback_result const_list_latch_visit_entry(const_list_latch *latch, size_t index, const generic_list *item);
const generic_list *const_list_latch_get_last_seen(const_list_latch *latch);

#endif /* TOY_LIST_LATCH_H */
