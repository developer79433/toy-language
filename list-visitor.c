#include "list-visitor.h"
#include "generic-list.h"

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

enumeration_result list_visitor_visit_list_default(list_visitor *visitor, generic_list *list)
{
    for (size_t i = 0; list; i++) {
        generic_list *next = list_next(list);
        item_callback_result res = list_visitor_visit_entry(visitor, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

enumeration_result list_visitor_visit_list(list_visitor *visitor, generic_list *list)
{
    if (visitor->visit_list) {
        return visitor->visit_list(visitor, list);
    }
    return list_visitor_visit_list_default(visitor, list);
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

enumeration_result const_list_visitor_visit_list_default(const_list_visitor *visitor, const generic_list *list)
{
    for (size_t i = 0; list; i++) {
        const generic_list *next = list_next_const(list);
        item_callback_result res = const_list_visitor_visit_entry(visitor, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

enumeration_result const_list_visitor_visit_list(const_list_visitor *visitor, const generic_list *list)
{
    if (visitor->visit_list) {
        return visitor->visit_list(visitor, list);
    }
    return const_list_visitor_visit_list_default(visitor, list);
}
