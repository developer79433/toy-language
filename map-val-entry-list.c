#include <string.h>

#include "str.h"
#include "map-val-entry-list.h"
#include "map-buf.h"
#include "map-buf-entry-list.h"

map_val_entry_list *map_val_entry_list_alloc(toy_str key, toy_val *value)
{
    return (map_val_entry_list *) map_buf_entry_list_alloc(key, value, sizeof(*value));
}

map_val_entry *map_val_entry_list_payload(map_val_entry_list *list)
{
    return &list->entry;
}

/* TODO: Should go in generic_map */
toy_str map_val_entry_list_key(map_val_entry_list *list)
{
    return list->entry.key;
}

toy_val *map_val_entry_list_val(map_val_entry_list *list)
{
    return &list->entry.payload.value;
}

const map_val_entry *map_val_entry_list_payload_const(const map_val_entry_list *list)
{
    return &list->entry;
}

void map_val_entry_list_payload_set(map_val_entry_list *list, map_val_entry *entry)
{
    memcpy(map_val_entry_list_payload(list), entry, sizeof(*entry));
}

map_val_entry_list **map_val_get_bucket(map_val *map, toy_str key)
{
    return (map_val_entry_list **) map_buf_get_bucket((map_buf *) map, key);
}
