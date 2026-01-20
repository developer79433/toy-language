#ifndef TOY_GENERIC_MAP_TYPES_H
#define TOY_GENERIC_MAP_TYPES_H 1

#include <stddef.h>

#include "bool-types.h"
#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"
#include "buf-list-types.h"
#include "generic-map-entry-list-types.h"

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

typedef struct generic_map_struct {
    size_t num_items;
    generic_map_entry_list *buckets[NUM_BUCKETS];
} generic_map;

typedef item_callback_result (*generic_map_entry_callback)(void *cookie, generic_map_entry *entry);
typedef item_callback_result (*const_generic_map_entry_callback)(void *cookie, const generic_map_entry *entry);

typedef toy_bool (*generic_map_filter_func)(void *cookie, const generic_map_entry *entry);

typedef struct map_filter_args_struct {
    generic_map_filter_func filter;
    generic_map_entry_callback user_callback;
    void *user_cookie;
} map_filter_args;

struct map_visitor_struct;
typedef struct map_visitor_struct map_visitor;
typedef item_callback_result (*map_entry_visit_func)(map_visitor *visitor, generic_map_entry *entry);
struct map_visitor_struct {
    map_entry_visit_func visit;
};

struct const_map_visitor_struct;
typedef struct const_map_visitor_struct const_map_visitor;
typedef item_callback_result (*const_map_entry_visit_func)(const_map_visitor *visitor, const generic_map_entry *entry);
struct const_map_visitor_struct {
    const_map_entry_visit_func visit;
};

struct map_filter_struct;
typedef struct map_filter_struct map_filter;
typedef item_callback_result (*map_filter_entry_visit_func)(map_filter *visitor, generic_map_entry *entry);
struct map_filter_struct {
    map_filter_entry_visit_func visit;
    generic_map_filter_func filter;
    void *filter_cookie;
};

#endif /* TOY_GENERIC_MAP_TYPES_H */
