#include <string.h>
#include <assert.h>
#include <wchar.h>

#include "str.h"
#include "map-str-val-entry-list.h"
#include "map-buf-buf.h"
#include "map-buf-buf-entry-list.h"
#include "val.h"
#include "log.h"

map_str_val_entry_list *map_str_val_entry_list_alloc(toy_str key, toy_val *value)
{
    map_str_val_entry_list *list = (map_str_val_entry_list *) map_buf_entry_list_alloc(key, sizeof(wchar_t) * (wcslen(key) + 1), value, sizeof(*value));
    assert(str_equal(list->entry.key, key));
    assert(list->entry.key_len == wcslen(key) * sizeof(wchar_t));
    assert(0 == memcmp(&list->entry.value, value, sizeof(*value)));
    return list;
}

map_str_val_entry *map_str_val_entry_list_payload(map_str_val_entry_list *list)
{
    return &list->entry;
}

/* TODO: Should go in generic_map */
toy_str map_str_val_entry_list_key(map_str_val_entry_list *list)
{
    return list->entry.key;
}

toy_val *map_str_val_entry_list_val(map_str_val_entry_list *list)
{
    return &list->entry.value;
}

const map_str_val_entry *map_str_val_entry_list_payload_const(const map_str_val_entry_list *list)
{
    return &list->entry;
}

void map_str_val_entry_list_payload_set(map_str_val_entry_list *list, map_str_val_entry *entry)
{
    memcpy(map_str_val_entry_list_payload(list), entry, sizeof(*entry));
}

void map_str_val_entry_dump(const map_str_val_entry *entry)
{
    str_dump(entry->key, TOY_FALSE);
    log_debug(L": ");
    val_dump(&entry->value, TOY_TRUE);
}
