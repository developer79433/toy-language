#ifndef TOY_MAP_VISITOR_TYPES_H
#define TOY_MAP_VISITOR_TYPES_H 1

#include "generic-map-types.h"

struct map_visitor_struct;
typedef struct map_visitor_struct map_visitor;
typedef enumeration_result (*map_visit_func)(map_visitor *visitor, generic_map *map);
typedef item_callback_result (*map_entry_visit_func)(map_visitor *visitor, generic_map_entry *entry);
struct map_visitor_struct {
    map_visit_func visit_map;
    map_entry_visit_func visit_entry;
};

struct const_map_visitor_struct;
typedef struct const_map_visitor_struct const_map_visitor;
typedef enumeration_result (*const_map_visit_func)(const_map_visitor *visitor, const generic_map *map);
typedef item_callback_result (*const_map_entry_visit_func)(const_map_visitor *visitor, const generic_map_entry *entry);
struct const_map_visitor_struct {
    const_map_visit_func visit_map;
    const_map_entry_visit_func visit_entry;
};

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

#endif /* TOY_MAP_VISITOR_TYPES_H */
