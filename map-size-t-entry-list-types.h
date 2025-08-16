#ifndef TOY_MAP_SIZE_T_ENTRY_LIST_TYPES_H
#define TOY_MAP_SIZE_T_ENTRY_LIST_TYPES_H 1

#include <stddef.h>

#include "str-types.h"

typedef struct map_size_t_entry_struct {
    toy_str key;
    size_t s;
} map_size_t_entry;

typedef struct map_size_t_entry_list_struct {
    struct map_size_t_entry_list_struct *next;
    map_size_t_entry entry;
} map_size_t_entry_list;

#endif /* TOY_MAP_SIZE_T_ENTRY_LIST_TYPES_H */
