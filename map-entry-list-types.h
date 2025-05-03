#ifndef TOY_MAP_ENTRY_LIST_TYPES_H
#define TOY_MAP_ENTRY_LIST_TYPES_H 1

#include "map-entry-types.h"

typedef struct toy_map_entry_list_struct {
    struct toy_map_entry_list_struct *next;
    toy_map_entry entry;
} toy_map_entry_list;

#endif /* TOY_MAP_ENTRY_LIST_TYPES_H */
