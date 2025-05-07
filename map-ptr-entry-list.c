#include "str.h"
#include "map-ptr-entry-list.h"
#include "buf-list.h"

map_ptr_entry_list *map_ptr_entry_list_alloc(toy_str key, void *ptr)
{
    return (map_ptr_entry_list *) buf_list_alloc_2(&key, sizeof(key), &ptr, sizeof(ptr));
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
