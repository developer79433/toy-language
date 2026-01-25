#ifndef TOY_GENERIC_MAP_ENTRY_LIST_TYPES_H
#define TOY_GENERIC_MAP_ENTRY_LIST_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"
#include "str-types.h"

typedef struct generic_map_entry_struct {
    toy_str key;
} generic_map_entry;

typedef struct generic_map_entry_list_struct {
    struct generic_map_entry_list_struct *next;
    generic_map_entry entry;
} generic_map_entry_list;

#endif /* TOY_GENERIC_MAP_ENTRY_LIST_TYPES_H */
