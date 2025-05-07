#include <assert.h>
#include <string.h>

#include "map-buf-entry-list.h"

#include "buf-list.h"

map_buf_entry_list *map_buf_entry_list_alloc(const toy_str key, void *buf, size_t buf_size)
{
    map_buf_entry_list *entry_list = (map_buf_entry_list *) buf_list_alloc_2((void *) &key, sizeof(key), buf, buf_size);
    assert(entry_list->entry.key == key);
    assert(0 == memcmp(&entry_list->entry.c, buf, buf_size));
    assert(NULL == entry_list->next);
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

void map_buf_entry_list_payload_set_buf(map_buf_entry_list *list, void *buf, size_t buf_size)
{
    map_buf_entry *entry = map_buf_entry_list_payload(list);
    memcpy(&entry->c, buf, buf_size);
}
