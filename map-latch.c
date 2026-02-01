#include "map-latch.h"

item_callback_result map_latch_visit_entry(map_latch *latch, generic_map_entry *entry)
{
    latch->last_seen_entry = entry;
    return CONTINUE_ENUMERATION;
}

void map_latch_init(map_latch *latch)
{
    latch->visitor.visit_entry = (map_entry_visit_func) map_latch_visit_entry;
    latch->last_seen_entry = NULL;
}

generic_map_entry *map_latch_get_last_seen(map_latch *latch)
{
    return latch->last_seen_entry;
}

item_callback_result const_map_latch_visit_entry(const_map_latch *latch, const generic_map_entry *entry)
{
    latch->last_seen_entry = entry;
    return CONTINUE_ENUMERATION;
}

void const_map_latch_init(const_map_latch *latch)
{
    latch->visitor.visit_entry = (const_map_entry_visit_func) const_map_latch_visit_entry;
    latch->last_seen_entry = NULL;
}

const generic_map_entry *const_map_latch_get_last_seen(const_map_latch *latch)
{
    return latch->last_seen_entry;
}
