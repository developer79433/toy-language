#include "list-visitor.h"
#include "generic-list.h"
#include "log.h"

/* #define DEBUG_LIST_VISITOR */

generic_list *list_visitor_prev_item(list_visitor *visitor)
{
    return visitor->previous_item;
}

item_callback_result list_visitor_visit_entry_default(list_visitor *visitor, size_t index, generic_list *item)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result list_visitor_visit_entry(list_visitor *visitor, size_t index, generic_list *item)
{
    if (visitor->visit_entry) {
        return visitor->visit_entry(visitor, index, item);
    }
    return list_visitor_visit_entry_default(visitor, index, item);
}

enumeration_result list_visitor_visit_list(list_visitor *visitor, generic_list *list)
{
    visitor->previous_item = NULL;
#ifdef DEBUG_LIST_VISITOR
    log_printf_file(__FILE__, "set previous_item to %p\n", visitor->previous_item);
#endif /* DEBUG_LIST_VISITOR */
    for (size_t index = 0; list; index++) {
        generic_list *next = list_next(list);
        item_callback_result res = list_visitor_visit_entry(visitor, index, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        visitor->previous_item = list;
#ifdef DEBUG_LIST_VISITOR
    log_printf_file(__FILE__, "set previous_item to %p\n", visitor->previous_item);
#endif /* DEBUG_LIST_VISITOR */
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

const generic_list *const_list_visitor_previous_item(const_list_visitor *visitor)
{
    return visitor->previous_item;
}

item_callback_result const_list_visitor_visit_entry_default(const_list_visitor *visitor, size_t index, const generic_list *item)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result const_list_visitor_visit_entry(const_list_visitor *visitor, size_t index, const generic_list *item)
{
    if (visitor->visit_entry) {
        return visitor->visit_entry(visitor, index, item);
    }
    return const_list_visitor_visit_entry_default(visitor, index, item);
}

enumeration_result const_list_visitor_visit_list(const_list_visitor *visitor, const generic_list *list)
{
    visitor->previous_item = NULL;
    for (size_t i = 0; list; i++) {
        const generic_list *next = list_next_const(list);
        item_callback_result res = const_list_visitor_visit_entry(visitor, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        visitor->previous_item = list;
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

static item_callback_result always_stop(list_visitor *visitor, size_t index, generic_list *item)
{
    return STOP_ENUMERATION;
}

static item_callback_result always_continue(list_visitor *visitor, size_t index, generic_list *item)
{
    return CONTINUE_ENUMERATION;
}

list_visitor stop_visitor = { .visit_entry = always_stop };
const_list_visitor const_stop_visitor = { .visit_entry = (const_list_entry_visit_func) always_stop };
list_visitor continue_visitor = { .visit_entry = always_continue };
const_list_visitor const_continue_visitor = { .visit_entry = (const_list_entry_visit_func) always_continue };
