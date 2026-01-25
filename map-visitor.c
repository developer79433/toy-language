#include  <stddef.h>
#include <assert.h>

#include "map-visitor.h"
#include "generic-map.h"
#include "generic-map-entry-list.h"
#include "list-visitor.h"

item_callback_result map_visitor_visit_entry_default(map_visitor *visitor, generic_map_entry *entry)
{
    /* NOP_*/
    return CONTINUE_ENUMERATION;
}

item_callback_result map_visitor_visit_entry(map_visitor *visitor, generic_map_entry *entry)
{
    if (visitor->visit_entry) {
        return visitor->visit_entry(visitor, entry);
    }
    return map_visitor_visit_entry_default(visitor, entry);
}

typedef struct map_entry_visitor_struct {
    list_visitor list_vis;
    map_visitor *map_vis;
} map_entry_visitor;

static item_callback_result map_visitor_visit_entry_with_index(map_entry_visitor *map_entry_vis, size_t index, generic_map_entry_list *list)
{
    generic_map_entry *entry = generic_map_entry_list_payload(list);
    return map_visitor_visit_entry(map_entry_vis->map_vis, entry);
}

typedef struct map_bucket_visitor_struct {
    bucket_visitor bucket_vis;
    map_visitor *map_vis;
} map_bucket_visitor;

static item_callback_result map_bucket_visitor_visit_bucket(map_bucket_visitor *map_bucket_vis, generic_map_entry_list *bucket)
{
    map_entry_visitor map_entry_vis = { .list_vis.visit_entry = (list_entry_visit_func) map_visitor_visit_entry_with_index, .map_vis = map_bucket_vis->map_vis };
    enumeration_result enum_res = list_visitor_visit_list((list_visitor *) &map_entry_vis, (generic_list *) bucket);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result map_visitor_visit_map(map_visitor *visitor, generic_map *map)
{
    map_bucket_visitor map_bucket_vis = { .bucket_vis.visit_bucket = (bucket_visit_func) map_bucket_visitor_visit_bucket, .map_vis = visitor };
    return bucket_visitor_visit_map((bucket_visitor *) &map_bucket_vis, map);
}

item_callback_result const_map_visitor_visit_entry_default(const_map_visitor *visitor, const generic_map_entry *entry)
{
    /* NOP_*/
    return CONTINUE_ENUMERATION;
}

item_callback_result const_map_visitor_visit_entry(const_map_visitor *visitor, const generic_map_entry *entry)
{
    if (visitor->visit_entry) {
        return visitor->visit_entry(visitor, entry);
    }
    return const_map_visitor_visit_entry_default(visitor, entry);
}

typedef struct const_map_entry_visitor_struct {
    const_list_visitor list_vis;
    const_map_visitor *map_vis;
} const_map_entry_visitor;

static item_callback_result const_map_visitor_visit_entry_with_index(const_map_entry_visitor *map_entry_vis, size_t index, const generic_map_entry_list *list)
{
    const generic_map_entry *entry = generic_map_entry_list_payload_const(list);
    return const_map_visitor_visit_entry(map_entry_vis->map_vis, entry);
}

typedef struct const_map_bucket_visitor_struct {
    const_bucket_visitor bucket_vis;
    const_map_visitor *map_vis;
} const_map_bucket_visitor;

static item_callback_result const_map_bucket_visitor_visit_bucket(const_map_bucket_visitor *map_bucket_vis, const generic_map_entry_list *bucket)
{
    const_map_entry_visitor map_entry_vis = { .list_vis.visit_entry = (const_list_entry_visit_func) const_map_visitor_visit_entry_with_index, .map_vis = map_bucket_vis->map_vis };
    enumeration_result enum_res = const_list_visitor_visit_list((const_list_visitor *) &map_entry_vis, (const generic_list *) bucket);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result const_map_visitor_visit_map(const_map_visitor *visitor, const generic_map *map)
{
    const_map_bucket_visitor map_bucket_vis = { .bucket_vis.visit_bucket = (const_bucket_visit_func) const_map_bucket_visitor_visit_bucket, .map_vis = visitor };
    return const_bucket_visitor_visit_map((const_bucket_visitor *) &map_bucket_vis, map);
}
