#ifndef TOY_MAP_BUF_PTR_ENTRY_LIST_TYPES_H
#define TOY_MAP_BUF_PTR_ENTRY_LIST_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"
#include "str-types.h"

typedef struct map_buf_ptr_entry_struct {
    void *key;
    size_t key_len;
    void *ptr;
} map_buf_ptr_entry;

typedef struct map_buf_ptr_entry_list_struct {
    struct map_buf_ptr_entry_list_struct *next;
    map_buf_ptr_entry entry;
} map_buf_ptr_entry_list;

#endif /* TOY_MAP_BUF_PTR_ENTRY_LIST_TYPES_H */
