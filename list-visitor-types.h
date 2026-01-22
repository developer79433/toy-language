#ifndef TOY_LIST_VISITOR_TYPES_H
#define TOY_LIST_VISITOR_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"
#include "generic-list-types.h"

struct list_visitor_struct;
typedef struct list_visitor_struct list_visitor;

typedef enumeration_result (*list_visit_func)(list_visitor *visitor, generic_list *list);
typedef item_callback_result (*list_entry_visit_func)(list_visitor *visitor, size_t index, generic_list *item);

struct list_visitor_struct {
    list_visit_func visit_list;
    list_entry_visit_func visit_entry;
};

struct const_list_visitor_struct;
typedef struct const_list_visitor_struct const_list_visitor;

typedef enumeration_result (*const_list_visit_func)(const_list_visitor *visitor, const generic_list *list);
typedef item_callback_result (*const_list_entry_visit_func)(const_list_visitor *visitor, size_t index, const generic_list *item);

struct const_list_visitor_struct {
    const_list_visit_func visit_list;
    const_list_entry_visit_func visit_entry;
};

#endif /* TOY_LIST_VISITOR_TYPES_H */
