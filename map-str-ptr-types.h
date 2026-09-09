#ifndef TOY_MAP_STR_PTR_TYPES_H
#define TOY_MAP_STR_PTR_TYPES_H 1

#include <stddef.h>

#include "str-types.h"

typedef struct map_str_ptr_entry_struct {
    toy_str key;
    size_t key_len;
    void *ptr;
} map_str_ptr_entry;

typedef struct map_str_ptr_entry_list_struct {
    struct map_str_ptr_entry_list_struct *next;
    map_str_ptr_entry entry;
} map_str_ptr_entry_list;

typedef struct map_str_ptr_struct {
    size_t num_items;
    size_t num_buckets;
    map_str_ptr_entry_list **buckets;
} map_str_ptr;

#endif /* TOY_MAP_STR_PTR_TYPES_H */
