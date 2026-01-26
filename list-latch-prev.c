#include <assert.h>

#include "list-latch-prev.h"
#include "list-latch.h"
#include "log.h"

/* #define DEBUG_LIST_LATCH_PREV */

void list_latch_prev_init(list_latch_prev *latch_prev, generic_list *list, toy_bool stop_on_first)
{
    list_latch_init(&latch_prev->latch, stop_on_first);
    latch_prev->latch.visitor.visit_entry = (list_entry_visit_func) list_latch_prev_visit_entry;
    latch_prev->prev = NULL;
}

item_callback_result list_latch_prev_visit_entry(list_latch_prev *latch_prev, size_t index, generic_list *item)
{
    item_callback_result res = list_latch_visit_entry(&latch_prev->latch, index, item);
    if (STOP_ENUMERATION == res) {
#ifdef DEBUG_LIST_LATCH_PREV
        log_printf("list-latch-prev: Stopping\n");
#endif /* DEBUG_LIST_LATCH_PREV */
        return res;
    }
    assert(CONTINUE_ENUMERATION == res);
    latch_prev->prev = item;
#ifdef DEBUG_LIST_LATCH_PREV
    log_printf("list-latch-prev: Remembered previous item %p, continuing\n", latch_prev->prev);
#endif /* DEBUG_LIST_LATCH_PREV */
    return res;
}
