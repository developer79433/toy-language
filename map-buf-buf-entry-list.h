#ifndef TOY_MAP_BUF_BUF_ENTRY_LIST_H
#define TOY_MAP_BUF_BUF_ENTRY_LIST_H 1

#include <stddef.h>

#include "map-buf-buf-entry-list-types.h"

map_buf_buf_entry_list *map_buf_entry_list_alloc(const void *key, size_t key_len, const void *buf, size_t buf_size);
map_buf_buf_entry *map_buf_entry_list_payload(map_buf_buf_entry_list *list);
const map_buf_buf_entry *map_buf_entry_list_payload_const(const map_buf_buf_entry_list *list);
void map_buf_entry_list_payload_set(map_buf_buf_entry_list *list, const void *buf, size_t buf_size);
#define map_buf_entry_list_payload_typed(list, type) ((type *) map_buf_entry_list_payload(list))

#endif /* TOY_MAP_BUF_BUF_ENTRY_LIST_H */
