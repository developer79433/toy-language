#ifndef TOY_REV_LIST_VISITOR_TYPES_H
#define TOY_REV_LIST_VISITOR_TYPES_H

#include <stddef.h>

#include "list-visitor-types.h"

typedef struct rev_list_visitor_struct {
    list_visitor list_vis;
    size_t index;
} rev_list_visitor;

typedef struct const_rev_list_visitor_struct {
    const_list_visitor list_vis;
    size_t index;
} const_rev_list_visitor;

#endif /* TOY_REV_LIST_VISITOR_TYPES_H */
