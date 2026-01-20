#ifndef GENERIC_LIST_TYPES_H
#define GENERIC_LIST_TYPES_H 1

#include <stddef.h>
#include <inttypes.h>

#include "bool-types.h"
#include "iter-types.h"

typedef struct generic_list_struct {
    struct generic_list_struct *next;
    /* user code should declare additional members in 'subclasses' here */
} generic_list;

typedef item_callback_result (*generic_list_item_callback)(void *cookie, size_t index, generic_list *item);
typedef item_callback_result (*const_generic_list_item_callback)(void *cookie, size_t index, const generic_list *item);

extern void *INDEX_OUT_OF_BOUNDS;

typedef toy_bool (*generic_list_filter_func)(void *cookie, size_t index, const generic_list *item);

typedef struct filter_args_struct {
    generic_list_filter_func filter;
    void *filter_cookie;
    generic_list_item_callback user_callback;
    void *user_cookie;
} filter_args;

typedef struct const_filter_args_struct {
    generic_list_filter_func filter;
    void *filter_cookie;
    const_generic_list_item_callback user_callback;
    void *user_cookie;
} const_filter_args;

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

#endif /* GENERIC_LIST_TYPES_H */
