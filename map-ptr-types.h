#ifndef TOY_MAP_PTR_TYPES_H
#define TOY_MAP_PTR_TYPES_H

#include "str-types.h"

typedef struct map_ptr_entry_struct {
    toy_str key;
    void *ptr;
} map_ptr_entry;

typedef struct map_ptr_entry_list_struct {
    struct map_ptr_entry_list_struct *next;
    map_ptr_entry entry;
} map_ptr_entry_list;

#endif /* TOY_MAP_PTR_TYPES_H */
