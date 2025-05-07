#ifndef TOY_MAP_PTR_ENTRY_LIST_H
#define TOY_MAP_PTR_ENTRY_LIST_H 1

#include "str-types.h"
#include "map-ptr-entry-list-types.h"

map_ptr_entry_list *map_ptr_entry_list_alloc(const toy_str key, void *ptr);
enumeration_result map_ptr_entry_list_foreach(map_ptr_entry_list *list, map_ptr_entry_list_item_callback callback, void *cookie);
enumeration_result map_ptr_entry_list_foreach_const(const map_ptr_entry_list *list, const_map_ptr_entry_list_item_callback callback, void *cookie);
map_ptr_entry *map_ptr_entry_list_payload(map_ptr_entry_list *list);
const map_ptr_entry *map_ptr_entry_list_payload_const(map_ptr_entry_list *list);
void map_ptr_entry_list_payload_set(map_ptr_entry_list *list, void *ptr);

#endif /* TOY_MAP_PTR_ENTRY_LIST_H */
