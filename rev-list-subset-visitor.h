#ifndef TOY_REV_LIST_SUBSET_VISITOR_H
#define TOY_REV_LIST_SUBSET_VISITOR_H 1

#include "rev-list-subset-visitor-types.h"

void rev_list_subset_visitor_init(rev_list_subset_visitor *rev_list_sub_vis, list_entry_visit_func visit_func, generic_list *stop_at);
enumeration_result rev_list_subset_visitor_visit_list(rev_list_subset_visitor *rev_list_sub_vis, generic_list *list);
void const_rev_list_subset_visitor_init(const_rev_list_subset_visitor *rev_list_sub_vis, const_list_entry_visit_func visit_func, const generic_list *stop_at);
enumeration_result const_rev_list_subset_visitor_visit_list(const_rev_list_subset_visitor *rev_list_sub_vis, const generic_list *list);

#endif /* TOY_REV_LIST_SUBSET_VISITOR_H */
