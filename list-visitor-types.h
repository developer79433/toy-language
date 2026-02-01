#ifndef TOY_LIST_VISITOR_TYPES_H
#define TOY_LIST_VISITOR_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"
#include "generic-list-types.h"

struct list_visitor_struct;
typedef struct list_visitor_struct list_visitor;
typedef item_callback_result (*list_entry_visit_func)(list_visitor *visitor, size_t index, generic_list *item);
struct list_visitor_struct {
    list_entry_visit_func visit_entry;
    /* TODO: Move this into a list_prev_visitor */
    generic_list *previous_item;
    /* TODO: Track index here in the visitor instead of in callback arguments */
};

struct const_list_visitor_struct;
typedef struct const_list_visitor_struct const_list_visitor;
typedef item_callback_result (*const_list_entry_visit_func)(const_list_visitor *visitor, size_t index, const generic_list *item);
struct const_list_visitor_struct {
    const_list_entry_visit_func visit_entry;
    /* TODO: Move this into a list_prev_visitor */
    const generic_list *previous_item;
    /* TODO: Track index here in the visitor instead of in callback arguments */
};

#endif /* TOY_LIST_VISITOR_TYPES_H */
