#include "map-latch.h"

static item_callback_result latch_visit_entry(map_latch *latch, generic_map_entry *entry)
{
    latch->last_seen_entry = entry;
    return latch->stop_on_first ? STOP_ENUMERATION : CONTINUE_ENUMERATION;
}

void map_latch_init(map_latch *latch, toy_bool stop_on_first)
{
    latch->visitor.visit_entry = (map_entry_visit_func) latch_visit_entry;
    latch->stop_on_first = stop_on_first;
    latch->last_seen_entry = NULL;
}

generic_map_entry *map_latch_get_last_seen(map_latch *latch)
{
    return latch->last_seen_entry;
}
