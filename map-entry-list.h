#ifndef TOY_MAP_ENTRY_LIST_H
#define TOY_MAP_ENTRY_LIST_H 1

#include <stdio.h>

#include "map-entry-list-types.h"

toy_map_entry_list *map_entry_list_concat(toy_map_entry_list *orig, toy_map_entry_list *new_list);
toy_map_entry_list *map_entry_list_alloc_ref(toy_str first_key, toy_expr *first_value);
void map_entry_list_dump(FILE *f, const toy_map_entry_list *list);

#endif /* TOY_MAP_ENTRY_LIST_H */
