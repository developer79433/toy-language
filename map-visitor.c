#include  <stddef.h>
#include <assert.h>

#include "map-visitor.h"
#include "generic-map.h"
#include "generic-map-entry-list.h"

static item_callback_result generic_map_visit_entry(void *cookie, size_t index, generic_map_entry_list *list)
{
    map_visitor *visitor = (map_visitor *) cookie;
    generic_map_entry *entry = generic_map_entry_list_payload(list);
    visitor->visit(visitor, entry);
    return CONTINUE_ENUMERATION;
}

typedef struct bucket_visit_args_struct {
    map_visitor *visitor;
} bucket_visit_args;

static item_callback_result generic_map_visit_bucket(void *cookie, generic_map_entry_list *bucket)
{
    bucket_visit_args *args = (bucket_visit_args *) cookie;
    enumeration_result enum_res = generic_map_entry_list_foreach(bucket, generic_map_visit_entry, args->visitor);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result map_visitor_visit(map_visitor *visitor, generic_map *map)
{
    bucket_visit_args args = { .visitor = visitor };
    return generic_map_enum_buckets(map, generic_map_visit_bucket, &args);
}

static item_callback_result generic_map_visit_entry_const(void *cookie, size_t index, const generic_map_entry_list *list)
{
    const_map_visitor *visitor = (const_map_visitor *) cookie;
    const generic_map_entry *entry = generic_map_entry_list_payload_const(list);
    visitor->visit(visitor, entry);
    return CONTINUE_ENUMERATION;
}

typedef struct const_bucket_visit_args_struct {
    const_map_visitor *visitor;
} const_bucket_visit_args;

static item_callback_result generic_map_visit_bucket_const(void *cookie, const generic_map_entry_list *bucket)
{
    const_bucket_visit_args *args = (const_bucket_visit_args *) cookie;
    enumeration_result enum_res = generic_map_entry_list_foreach_const(bucket, generic_map_visit_entry_const, args->visitor);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result const_map_visitor_visit(const_map_visitor *visitor, const generic_map *map)
{
    const_bucket_visit_args args = { .visitor = visitor };
    return generic_map_enum_buckets_const(map, generic_map_visit_bucket_const, &args);
}
