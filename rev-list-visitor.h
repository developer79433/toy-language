#ifndef TOY_REV_LIST_VISITOR_H
#define TOY_REV_LIST_VISITOR_H 1

#include "rev-list-visitor-types.h"
#include "iter-types.h"
#include "generic-list-types.h"

void rev_list_visitor_init(rev_list_visitor *rev_list_vis, list_entry_visit_func visit_func);
enumeration_result rev_list_visitor_visit_list(rev_list_visitor *rev_list_vis, generic_list *list);
void const_rev_list_visitor_init(const_rev_list_visitor *rev_list_vis, const_list_entry_visit_func visit_func);
enumeration_result const_rev_list_visitor_visit_list(const_rev_list_visitor *rev_list_vis, const generic_list *list);

#endif /* TOY_REV_LIST_VISITOR_H */
