#ifndef TOY_BUF_LIST_H
#define TOY_BUF_LIST_H 1

#include <stddef.h>

#include "buf-list-types.h"

toy_buf_list *buf_list_alloc(void *buf, size_t buf_size);
void buf_list_free(toy_buf_list *list);
toy_buf_list *buf_list_concat(toy_buf_list *list, toy_buf_list *new_list);
toy_buf_list *buf_list_append(toy_buf_list *list, void *buf, size_t buf_size);
size_t buf_list_len(const toy_buf_list *list);
list_iter_result buf_list_index(toy_buf_list *list, size_t index, buf_list_item_callback callback, void *cookie);
void buf_list_foreach(toy_buf_list *list, buf_list_item_callback callback, void *cookie);
void buf_list_foreach_const(const toy_buf_list *list, const_buf_list_item_callback callback, void *cookie);
void *buf_list_payload_get_raw(toy_buf_list *list);
#define buf_list_payload_get_typed(list, type) ((type) buf_list_payload_get_raw(list))

#endif /* TOY_BUF_LIST_H */
