#ifndef TOY_MAP_BUF_ENTRY_LIST_TYPES_H
#define TOY_MAP_BUF_ENTRY_LIST_TYPES_H 1

#include <inttypes.h>

#include "iter-types.h"
#include "str-types.h"

typedef struct map_buf_payload_struct {
    uint8_t c; /* Really varable-size */
} map_buf_payload;

typedef struct map_buf_entry_struct {
    toy_str key;
    map_buf_payload payload;
} map_buf_entry;

typedef struct map_buf_entry_list_struct {
    struct map_buf_entry_list_struct *next;
    map_buf_entry entry;
} map_buf_entry_list;

typedef item_callback_result (*map_buf_entry_list_item_callback)(void *cookie, size_t index, map_buf_entry_list *item);
typedef item_callback_result (*const_map_buf_entry_list_item_callback)(void *cookie, size_t index, const map_buf_entry_list *item);

#endif /* TOY_MAP_BUF_ENTRY_LIST_TYPES_H */
