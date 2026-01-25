#ifndef TOY_MAP_ENTRY_LIST_TYPES_H
#define TOY_MAP_ENTRY_LIST_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"
#include "map-entry-types.h"

/* TODO: This needs a name that makes it seem more like a list of map-entries and less like a map of entry-lists */
typedef struct toy_map_entry_list_struct {
    struct toy_map_entry_list_struct *next;
    toy_map_entry entry;
} toy_map_entry_list;

#endif /* TOY_MAP_ENTRY_LIST_TYPES_H */
