#ifndef TOY_LIST_FILTER_TYPES_H
#define TOY_LIST_FILTER_TYPES_H 1

#include "generic-list-types.h"
#include "list-visitor-types.h"

struct list_filter_struct;
typedef struct list_filter_struct list_filter;
struct list_filter_struct {
    list_visitor visitor;
    generic_list_filter_func filter_func;
    void *filter_cookie;
    toy_bool inverted;
    list_visitor *next_visitor;
};

struct const_list_filter_struct;
typedef struct const_list_filter_struct const_list_filter;
struct const_list_filter_struct {
    const_list_visitor visitor;
    generic_list_filter_func filter_func;
    void *filter_cookie;
    toy_bool inverted;
    const_list_visitor *next_visitor;
};

#endif /* TOY_LIST_FILTER_TYPES_H */
