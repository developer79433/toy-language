#ifndef TOY_MAP_ENTRY_LIST_H
#define TOY_MAP_ENTRY_LIST_H 1

#include <stdio.h>

#include "map-entry-list-types.h"

/* TODO: Should be named "toy_mapexpr_entry_list" */
toy_map_entry_list *map_entry_list_concat(toy_map_entry_list *orig, toy_map_entry_list *new_list);
toy_map_entry_list *map_entry_list_alloc_ref(toy_str first_key, toy_expr *first_value);
enumeration_result map_entry_list_foreach(toy_map_entry_list *list, toy_map_entry_list_item_callback callback, void *cookie);
enumeration_result map_entry_list_foreach_const(const toy_map_entry_list *list, const_toy_map_entry_list_item_callback callback, void *cookie);
toy_map_entry *map_entry_list_payload(toy_map_entry_list *list);
const toy_map_entry *map_entry_list_payload_const(const toy_map_entry_list *list);
void map_entry_list_dump(FILE *f, const toy_map_entry_list *list);

#endif /* TOY_MAP_ENTRY_LIST_H */
