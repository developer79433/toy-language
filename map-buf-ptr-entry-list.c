#include <assert.h>
#include <string.h>

#include "map-buf-ptr-entry-list.h"
#include "map-buf-entry-list.h"

map_buf_ptr_entry_list *map_buf_ptr_entry_list_alloc(const void *key, size_t key_len, void *ptr)
{
    map_buf_ptr_entry_list *entry_list = (map_buf_ptr_entry_list *) map_buf_entry_list_alloc(key, key_len, &ptr, sizeof(ptr));
    assert(entry_list->entry.key == key);
    assert(entry_list->entry.ptr == ptr);
    assert(NULL == entry_list->next);
    return entry_list;
}

map_buf_ptr_entry *map_buf_ptr_entry_list_payload(map_buf_ptr_entry_list *list)
{
    return (map_buf_ptr_entry *) map_buf_entry_list_payload((map_buf_entry_list *) list);
}

const map_buf_ptr_entry *map_buf_ptr_entry_list_payload_const(map_buf_ptr_entry_list *list)
{
    return (const map_buf_ptr_entry *) map_buf_entry_list_payload_const((const map_buf_entry_list *) list);
}

void map_buf_ptr_entry_list_payload_set(map_buf_ptr_entry_list *list, void *ptr)
{
    return map_buf_entry_list_payload_set((map_buf_entry_list *) list, &ptr, sizeof(ptr));
}
