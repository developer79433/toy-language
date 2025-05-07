
#ifndef TOY_MAP_BUF_ENTRY_LIST_H
#define TOY_MAP_BUF_ENTRY_LIST_H 1

#include <stddef.h>

#include "map-buf-entry-list-types.h"

map_buf_entry_list *map_buf_entry_list_alloc(const toy_str key, void *buf, size_t buf_size);
enumeration_result map_buf_entry_list_foreach(map_buf_entry_list *list, map_buf_entry_list_item_callback callback, void *cookie);
enumeration_result map_buf_entry_list_foreach_const(const map_buf_entry_list *list, const_map_buf_entry_list_item_callback callback, void *cookie);
map_buf_entry *map_buf_entry_list_payload(map_buf_entry_list *list);
const map_buf_entry *map_buf_entry_list_payload_const(const map_buf_entry_list *list);
void map_buf_entry_list_payload_set(map_buf_entry_list *list, void *buf, size_t buf_size);
#define map_buf_entry_list_payload_typed(list, type) ((type *) map_buf_entry_list_payload(list))

#endif /* TOY_MAP_BUF_ENTRY_LIST_H */
