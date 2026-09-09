#ifndef TOY_MAP_BUF_PTR_ENTRY_LIST_H
#define TOY_MAP_BUF_PTR_ENTRY_LIST_H 1

#include "map-buf-ptr-entry-list-types.h"

map_buf_ptr_entry_list *map_buf_ptr_entry_list_alloc(const void *key, size_t key_len, void *ptr);
map_buf_ptr_entry *map_buf_ptr_entry_list_payload(map_buf_ptr_entry_list *list);
const map_buf_ptr_entry *map_buf_ptr_entry_list_payload_const(map_buf_ptr_entry_list *list);
void map_buf_ptr_entry_list_payload_set(map_buf_ptr_entry_list *list, void *ptr);

#endif /* TOY_MAP_BUF_PTR_ENTRY_LIST_H */
