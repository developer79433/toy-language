#include  <stddef.h>
#include <assert.h>

#include "map-visitor.h"
#include "generic-map.h"
#include "generic-map-entry-list.h"

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

static item_callback_result map_visitor_visit_entry_with_index(void *cookie, size_t index, generic_map_entry_list *list)
{
    map_visitor *visitor = (map_visitor *) cookie;
    generic_map_entry *entry = generic_map_entry_list_payload(list);
    return map_visitor_visit_entry(visitor, entry);
}

item_callback_result map_visitor_visit_bucket_default(map_visitor *visitor, generic_map_entry_list *bucket)
{
    enumeration_result enum_res = generic_map_entry_list_foreach(bucket, map_visitor_visit_entry_with_index, visitor);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

item_callback_result map_visitor_visit_bucket(map_visitor *visitor, generic_map_entry_list *bucket)
{
    if (visitor->visit_bucket) {
        return visitor->visit_bucket(visitor, bucket);
    }
    return map_visitor_visit_bucket_default(visitor, bucket);
}

enumeration_result map_visitor_visit_map_default(map_visitor *visitor, generic_map *map)
{
    return generic_map_enum_buckets(map, (generic_map_bucket_callback) map_visitor_visit_bucket, visitor);
}

enumeration_result map_visitor_visit_map(map_visitor *visitor, generic_map *map)
{
    if (visitor->visit_map) {
        return visitor->visit_map(visitor, map);
    }
    return map_visitor_visit_map_default(visitor, map);
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

static item_callback_result const_map_visitor_visit_entry_with_index(void *cookie, size_t index, const generic_map_entry_list *list)
{
    const_map_visitor *visitor = (const_map_visitor *) cookie;
    const generic_map_entry *entry = generic_map_entry_list_payload_const(list);
    return const_map_visitor_visit_entry(visitor, entry);
}

item_callback_result const_map_visitor_visit_bucket_default(const_map_visitor *visitor, const generic_map_entry_list *bucket)
{
    enumeration_result enum_res = generic_map_entry_list_foreach_const(bucket, const_map_visitor_visit_entry_with_index, visitor);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

item_callback_result const_map_visitor_visit_bucket(const_map_visitor *visitor, const generic_map_entry_list *bucket)
{
    if (visitor->visit_bucket) {
        return visitor->visit_bucket(visitor, bucket);
    }
    return const_map_visitor_visit_bucket_default(visitor, bucket);
}

enumeration_result const_map_visitor_visit_map_default(const_map_visitor *visitor, const generic_map *map)
{
    return generic_map_enum_buckets_const(map, (const_generic_map_bucket_callback) const_map_visitor_visit_bucket, visitor);
}

enumeration_result const_map_visitor_visit_map(const_map_visitor *visitor, const generic_map *map)
{
    if (visitor->visit_map) {
        return visitor->visit_map(visitor, map);
    }
    return const_map_visitor_visit_map_default(visitor, map);
}
