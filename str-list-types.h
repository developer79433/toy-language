#ifndef TOY_STR_LIST_TYPES_H
#define TOY_STR_LIST_TYPES_H 1

#include <stddef.h>

#include "str-types.h"
#include "generic-list-types.h"

typedef struct toy_str_list_struct {
    struct toy_str_list_struct *next;
    toy_str str;
} toy_str_list;

typedef item_callback_result (*toy_str_list_item_callback)(void *cookie, size_t index, toy_str_list *list);
typedef item_callback_result (*const_toy_str_list_item_callback)(void *cookie, size_t index, const toy_str_list *list);

typedef toy_bool (*str_list_filter_func)(void *cookie, size_t index, const toy_str_list *item);

struct str_list_visitor_struct;
typedef struct str_list_visitor_struct str_list_visitor;

typedef enumeration_result (*str_list_visit_func)(str_list_visitor *visitor, toy_str_list *list);
typedef item_callback_result (*str_list_entry_visit_func)(str_list_visitor *visitor, size_t index, toy_str_list *item);

struct str_list_visitor_struct {
    str_list_visit_func visit_list;
    str_list_entry_visit_func visit_entry;
};

struct const_str_list_visitor_struct;
typedef struct const_str_list_visitor_struct const_str_list_visitor;

typedef enumeration_result (*const_str_list_visit_func)(const_str_list_visitor *visitor, const toy_str_list *list);
typedef item_callback_result (*const_str_list_entry_visit_func)(const_str_list_visitor *visitor, size_t index, const toy_str_list *item);

struct const_str_list_visitor_struct {
    const_str_list_visit_func visit_list;
    const_str_list_entry_visit_func visit_entry;
};

#endif /* TOY_STR_LIST_TYPES_H */
