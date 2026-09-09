#include <string.h>
#include <assert.h>

#include "str.h"
#include "map-val-entry-list.h"
#include "map-buf.h"
#include "map-buf-entry-list.h"
#include "val.h"
#include "log.h"

map_val_entry_list *map_val_entry_list_alloc(toy_str key, toy_val *value)
{
    map_val_entry_list *list = (map_val_entry_list *) map_buf_entry_list_alloc(key, strlen(key) + 1, value, sizeof(*value));
    assert(str_equal(list->entry.key, key));
    assert(list->entry.key_len == strlen(key));
    assert(0 == memcmp(&list->entry.value, value, sizeof(*value)));
    return list;
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
    return &list->entry.value;
}

const map_val_entry *map_val_entry_list_payload_const(const map_val_entry_list *list)
{
    return &list->entry;
}

void map_val_entry_list_payload_set(map_val_entry_list *list, map_val_entry *entry)
{
    memcpy(map_val_entry_list_payload(list), entry, sizeof(*entry));
}

void map_val_entry_dump(const map_val_entry *entry)
{
    str_dump(entry->key, TOY_FALSE);
    log_debug(": ");
    val_dump(&entry->value, TOY_TRUE);
}
