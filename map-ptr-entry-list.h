#ifndef TOY_MAP_PTR_ENTRY_LIST_H
#define TOY_MAP_PTR_ENTRY_LIST_H 1

#include "str-types.h"
#include "map-ptr-entry-list-types.h"

map_ptr_entry_list *map_ptr_entry_list_alloc(const toy_str key, void *ptr);
map_ptr_entry *map_ptr_entry_list_payload(map_ptr_entry_list *list);
const map_ptr_entry *map_ptr_entry_list_payload_const(map_ptr_entry_list *list);
void map_ptr_entry_list_payload_set(map_ptr_entry_list *list, void *ptr);

#endif /* TOY_MAP_PTR_ENTRY_LIST_H */
