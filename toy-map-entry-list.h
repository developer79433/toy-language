#ifndef TOY_MAP_ENTRY_LIST_H
#define TOY_MAP_ENTRY_LIST_H 1

#include <stdio.h>

#include "toy-map-entry-list-types.h"

toy_map_entry_list *append_map_entry(toy_map_entry_list *orig, toy_map_entry_list *new_entry);
toy_map_entry_list *alloc_map_entry_list(toy_str first_key, toy_expr *first_value);
void dump_map_entry_list(FILE *f, const toy_map_entry_list *list);

#endif /* TOY_MAP_ENTRY_LIST_H */
