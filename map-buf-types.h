#ifndef TOY_MAP_BUF_TYPES_H
#define TOY_MAP_BUF_TYPES_H

#include <inttypes.h>

#include "str-types.h"

typedef struct map_buf_entry_struct {
    toy_str key;
    uint8_t c; /* Really varable-size */
} map_buf_entry;

typedef struct map_buf_entry_list_struct {
    struct map_buf_entry_list_struct *next;
    map_buf_entry entry;
} map_buf_entry_list;

#endif /* TOY_MAP_BUF_TYPES_H */
