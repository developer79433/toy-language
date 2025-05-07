#include <assert.h>

#include "str.h"
#include "map-ptr-entry-list.h"
#include "buf-list.h"
#include "generic-map-entry-list.h"

map_ptr_entry_list *map_ptr_entry_list_alloc(const toy_str key, void *ptr)
{
    map_ptr_entry_list *entry_list = (map_ptr_entry_list *) generic_map_entry_list_alloc(key, &ptr, sizeof(ptr));
    assert(entry_list->entry.key == key);
    assert(entry_list->entry.ptr == ptr);
    assert(NULL == entry_list->next);
    return entry_list;
}

map_ptr_entry *map_ptr_entry_list_payload(map_ptr_entry_list *list)
{
    return (map_ptr_entry *) buf_list_payload((toy_buf_list *) list);
}

const map_ptr_entry *map_ptr_entry_list_payload_const(map_ptr_entry_list *list)
{
    return (map_ptr_entry *) buf_list_payload((toy_buf_list *) list);
}

void map_ptr_entry_list_payload_set(map_ptr_entry_list *list, void *ptr)
{
    map_ptr_entry_list_payload(list)->ptr = ptr;
}

enumeration_result map_ptr_entry_list_foreach(map_ptr_entry_list *list, map_ptr_entry_list_item_callback callback, void *cookie)
{
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);
}

enumeration_result map_ptr_entry_list_foreach_const(const map_ptr_entry_list *list, const_map_ptr_entry_list_item_callback callback, void *cookie)
{
    return buf_list_foreach_const((const toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);
}
