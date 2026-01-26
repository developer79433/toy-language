#ifndef TOY_GENERIC_MAP_TYPES_H
#define TOY_GENERIC_MAP_TYPES_H 1

#include <stddef.h>

#include "bool-types.h"
#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"
#include "buf-list-types.h"
#include "generic-map-entry-list-types.h"

typedef struct generic_map_struct {
    size_t num_items;
    size_t num_buckets;
    generic_map_entry_list **buckets;
} generic_map;

typedef item_callback_result (*generic_map_entry_callback)(void *cookie, generic_map_entry *entry);
typedef item_callback_result (*const_generic_map_entry_callback)(void *cookie, const generic_map_entry *entry);

typedef item_callback_result (*generic_map_bucket_callback)(void *cookie, generic_map_entry_list *bucket);
typedef item_callback_result (*const_generic_map_bucket_callback)(void *cookie, const generic_map_entry_list *bucket);

typedef toy_bool (*generic_map_filter_func)(void *cookie, const generic_map_entry *entry);

typedef struct map_filter_args_struct {
    generic_map_filter_func filter;
    generic_map_entry_callback user_callback;
    void *user_cookie;
} map_filter_args;

struct bucket_visitor_struct;
typedef struct bucket_visitor_struct bucket_visitor;
typedef item_callback_result (*bucket_visit_func)(bucket_visitor *visitor, generic_map_entry_list *bucket);
struct bucket_visitor_struct {
    bucket_visit_func visit_bucket;
};

struct const_bucket_visitor_struct;
typedef struct const_bucket_visitor_struct const_bucket_visitor;
typedef item_callback_result (*const_bucket_visit_func)(const_bucket_visitor *visitor, const generic_map_entry_list *bucket);
struct const_bucket_visitor_struct {
    const_bucket_visit_func visit_bucket;
};

#endif /* TOY_GENERIC_MAP_TYPES_H */
