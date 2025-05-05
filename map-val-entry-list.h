#ifndef TOY_MAP_VAL_ENTRY_LIST_H
#define TOY_MAP_VAL_ENTRY_LIST_H 1

#include "map-val-entry-list-types.h"

map_val_entry_list *map_val_entry_list_alloc(toy_str key, toy_val *value);
map_val_entry *map_val_entry_list_payload(map_val_entry_list *list);
const map_val_entry *map_val_entry_list_payload_const(const map_val_entry_list *list);
void map_val_entry_list_payload_set(map_val_entry_list *list, map_val_entry *entry);
toy_str map_val_entry_list_key(map_val_entry_list *list);
toy_val *map_val_entry_list_val(map_val_entry_list *list);

#endif /* TOY_MAP_VAL_ENTRY_LIST_H */
