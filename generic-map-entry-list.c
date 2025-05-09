#include <assert.h>

#include "generic-map-entry-list.h"
#include "buf-list.h"

generic_map_entry_list *generic_map_entry_list_alloc(const toy_str key, const void *buf, size_t buf_size)
{
    generic_map_entry_list *list = (generic_map_entry_list *) buf_list_alloc_2(&key, sizeof(key), buf, buf_size);
    assert(NULL == list->next);
    generic_map_entry *entry = generic_map_entry_list_payload(list);
    assert(entry);
    assert(entry->key);
    assert(entry->key == key);
    return list;
}

generic_map_entry *generic_map_entry_list_payload(generic_map_entry_list *list)
{
    return buf_list_payload_typed((toy_buf_list *) list, generic_map_entry);
}

const generic_map_entry *generic_map_entry_list_payload_const(const generic_map_entry_list *list)
{
    return buf_list_payload_const_typed((toy_buf_list *) list, generic_map_entry);
}

void generic_map_entry_list_free(generic_map_entry_list *list)
{
    return buf_list_free((toy_buf_list *) list);
}

enumeration_result generic_map_entry_list_foreach(generic_map_entry_list *list, generic_map_entry_list_item_callback callback, void *cookie)
{
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);
}

enumeration_result generic_map_entry_list_foreach_const(const generic_map_entry_list *list, const_generic_map_entry_list_item_callback callback, void *cookie)
{
    return buf_list_foreach_const((const toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);
}
