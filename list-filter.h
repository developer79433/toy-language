#ifndef TOY_LIST_FILTER_H
#define TOY_LIST_FILTER_H 1

#include "list-filter-types.h"

void list_filter_init(my_list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted, list_visitor *next_visitor);
enumeration_result list_filter_visit_list(my_list_filter *filter, generic_list *list);
item_callback_result list_filter_visit_entry(my_list_filter *filter, size_t index, generic_list *entry);
void const_list_filter_init(const_my_list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted, const_list_visitor *next_visitor);
enumeration_result const_list_filter_visit_list(const_my_list_filter *filter, const generic_list *list);
item_callback_result const_list_filter_visit_entry(const_my_list_filter *filter, size_t index, const generic_list *entry);

#endif /* TOY_LIST_FILTER_H */
