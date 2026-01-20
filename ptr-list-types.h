#ifndef TOY_PTR_LIST_TYPES_H
#define TOY_PTR_LIST_TYPES_H 1

#include "generic-list-types.h"

typedef struct toy_ptr_list_struct {
    struct toy_ptr_list_struct *next;
    void *ptr;
} toy_ptr_list;

typedef item_callback_result (*ptr_list_item_callback)(void *cookie, size_t index, toy_ptr_list *item);
typedef item_callback_result (*const_ptr_list_item_callback)(void *cookie, size_t index, const toy_ptr_list *item);

typedef toy_bool (*ptr_list_filter_func)(void *cookie, size_t index, const toy_ptr_list *item);

struct ptr_list_visitor_struct;
typedef struct ptr_list_visitor_struct ptr_list_visitor;

typedef enumeration_result (*ptr_list_visit_func)(ptr_list_visitor *visitor, toy_ptr_list *list);
typedef item_callback_result (*ptr_list_entry_visit_func)(ptr_list_visitor *visitor, size_t index, toy_ptr_list *item);

struct ptr_list_visitor_struct {
    ptr_list_visit_func visit_list;
    ptr_list_entry_visit_func visit_entry;
};

struct const_ptr_list_visitor_struct;
typedef struct const_ptr_list_visitor_struct const_ptr_list_visitor;

typedef enumeration_result (*const_ptr_list_visit_func)(const_ptr_list_visitor *visitor, const toy_ptr_list *list);
typedef item_callback_result (*const_ptr_list_entry_visit_func)(const_ptr_list_visitor *visitor, size_t index, const toy_ptr_list *item);

struct const_ptr_list_visitor_struct {
    const_ptr_list_visit_func visit_list;
    const_ptr_list_entry_visit_func visit_entry;
};

#endif /* TOY_PTR_LIST_TYPES_H */
