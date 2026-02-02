#ifndef TOY_REV_LIST_SUBSET_VISITOR_TYPES_H
#define TOY_REV_LIST_SUBSET_VISITOR_TYPES_H 1

#include "rev-list-visitor-types.h"
#include "generic-list-types.h"

typedef struct rev_list_subset_visitor_struct {
    rev_list_visitor rev_list_vis;
    generic_list *stop_at;
} rev_list_subset_visitor;

typedef struct const_rev_list_subset_visitor_struct {
    const_rev_list_visitor rev_list_vis;
    const generic_list *stop_at;
} const_rev_list_subset_visitor;

#endif /* TOY_REV_LIST_SUBSET_VISITOR_TYPES_H */
