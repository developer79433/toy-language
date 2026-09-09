#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "map-buf-entry-list.h"
#include "generic-map-entry-list.h"

#include "buf-list.h"
#include "str.h"

map_buf_entry_list *map_buf_entry_list_alloc(const toy_str key, const void *buf, size_t buf_size)
{
    map_buf_entry_list *entry_list = (map_buf_entry_list *) malloc(sizeof(map_buf_entry_list) + strlen(key) + 1 + buf_size);
    entry_list->next = NULL;
    entry_list->entry.key = key;
    entry_list->entry.key_len = strlen(key) + 1;
    memcpy(&entry_list->entry.c, buf, buf_size);
    return entry_list;
}

map_buf_entry *map_buf_entry_list_payload(map_buf_entry_list *list)
{
    return &list->entry;
}

const map_buf_entry *map_buf_entry_list_payload_const(const map_buf_entry_list *list)
{
    return &list->entry;
}

void map_buf_entry_list_payload_set(map_buf_entry_list *list, const void *buf, size_t buf_size)
{
    map_buf_entry *entry = map_buf_entry_list_payload(list);
    memcpy(&entry->c, buf, buf_size);
}
