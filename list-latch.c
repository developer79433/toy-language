#include "list-latch.h"
#include "log.h"

/* #define DEBUG_LIST_LATCH */

void list_latch_init(list_latch *latch)
{
    latch->visitor.visit_entry = (list_entry_visit_func) list_latch_visit_entry;
    latch->visitor.prev_item = NULL;
    latch->last_seen_item = NULL;
}

item_callback_result list_latch_visit_entry(list_latch *latch, size_t index, generic_list *item)
{
    latch->last_seen_item = item;
#ifdef DEBUG_LIST_LATCH
    log_printf_file(__, "remembered last-seen item %p\n", latch->last_seen_item);
#endif /* DEBUG_LIST_LATCH */
    return CONTINUE_ENUMERATION;
}

generic_list *list_latch_get_last_seen(list_latch *latch)
{
    return latch->last_seen_item;
}

void const_list_latch_init(const_list_latch *latch)
{
    latch->visitor.visit_entry = (const_list_entry_visit_func) list_latch_visit_entry;
    latch->visitor.prev_item = NULL;
    latch->last_seen_item = NULL;
}

item_callback_result const_list_latch_visit_entry(const_list_latch *latch, size_t index, const generic_list *item)
{
    latch->last_seen_item = item;
#ifdef DEBUG_LIST_LATCH
    log_printf_file(__, "remembered last-seen item %p\n", latch->last_seen_item);
#endif /* DEBUG_LIST_LATCH */
    return CONTINUE_ENUMERATION;
}

const generic_list *const_list_latch_get_last_seen(const_list_latch *latch)
{
    return latch->last_seen_item;
}
