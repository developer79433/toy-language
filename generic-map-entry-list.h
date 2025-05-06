#ifndef TOY_GENERIC_MAP_ENTRY_LIST_H
#define TOY_GENERIC_MAP_ENTRY_LIST_H 1

#include "generic-map-entry-list-types.h"

generic_map_entry *generic_map_entry_list_payload(generic_map_entry_list *list);
const generic_map_entry *generic_map_entry_list_payload_const(const generic_map_entry_list *list);
enumeration_result generic_map_entry_list_foreach(generic_map_entry_list *list, generic_map_entry_list_item_callback callback, void *cookie);
enumeration_result generic_map_entry_list_foreach_const(const generic_map_entry_list *list, const_generic_map_entry_list_item_callback callback, void *cookie);
void generic_map_entry_list_free(generic_map_entry_list *list);

#endif /* TOY_GENERIC_MAP_ENTRY_LIST_H */
