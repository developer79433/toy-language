#ifndef TOY_LIST_VISITOR_H
#define TOY_LIST_VISITOR_H 1

#include "generic-list-types.h"
#include "list-visitor-types.h"

enumeration_result list_visitor_visit_list(list_visitor *visitor, generic_list *list);
enumeration_result list_visitor_visit_list_default(list_visitor *visitor, generic_list *list);
enumeration_result const_list_visitor_visit_list(const_list_visitor *visitor, const generic_list *list);
enumeration_result const_list_visitor_visit_list_default(const_list_visitor *visitor, const generic_list *list);
item_callback_result list_visitor_visit_entry_default(list_visitor *visitor, size_t index, generic_list *item);
item_callback_result list_visitor_visit_entry(list_visitor *visitor, size_t index, generic_list *item);
item_callback_result const_list_visitor_visit_entry_default(const_list_visitor *visitor, size_t index, const generic_list *item);
item_callback_result const_list_visitor_visit_entry(const_list_visitor *visitor, size_t index, const generic_list *item);

#endif /* TOY_LIST_VISITOR_H */
