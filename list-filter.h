#ifndef TOY_LIST_FILTER_H
#define TOY_LIST_FILTER_H 1

#include "list-filter-types.h"

void list_filter_init(list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted);
item_callback_result list_filter_visit_entry(list_filter *filter, size_t index, generic_list *entry);
generic_list *list_filter_last_match(const list_filter *filter);
toy_bool list_filter_last_result(const list_filter *filter);
void const_list_filter_init(const_list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted);
item_callback_result const_list_filter_visit_entry(const_list_filter *filter, size_t index, const generic_list *entry);
toy_bool const_list_filter_last_result(const const_list_filter *filter);
const generic_list *const_list_filter_last_match(const const_list_filter *filter);

#endif /* TOY_LIST_FILTER_H */
