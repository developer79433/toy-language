#ifndef TOY_MAP_BUF_BUF_ENTRY_LIST_TYPES_H
#define TOY_MAP_BUF_BUF_ENTRY_LIST_TYPES_H 1

#include <inttypes.h>

#include "iter-types.h"
#include "str-types.h"

typedef struct map_buf_buf_entry_struct {
    void *key;
    size_t key_len;
    uint8_t c; /* Really varable-size */
} map_buf_buf_entry;

typedef struct map_buf_buf_entry_list_struct {
    struct map_buf_buf_entry_list_struct *next;
    map_buf_buf_entry entry;
} map_buf_buf_entry_list;

#endif /* TOY_MAP_BUF_BUF_ENTRY_LIST_TYPES_H */
