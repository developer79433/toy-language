#ifndef TOY_BUF_LIST_H
#define TOY_BUF_LIST_H 1

#include <stddef.h>

#include "buf-list-types.h"

toy_buf_list *buf_list_alloc(void *buf, size_t buf_size);
toy_buf_list *buf_list_alloc_2(void *buf1, size_t buf1_size, void *buf2, size_t buf2_size);
void buf_list_free(toy_buf_list *list);
toy_buf_list *buf_list_concat(toy_buf_list *list, toy_buf_list *new_list);
toy_buf_list *buf_list_append(toy_buf_list *list, void *buf, size_t buf_size);
size_t buf_list_len(const toy_buf_list *list);
void *buf_list_index(toy_buf_list *list, size_t index);
enumeration_result buf_list_foreach(toy_buf_list *list, buf_list_item_callback callback, void *cookie);
enumeration_result buf_list_foreach_const(const toy_buf_list *list, const_buf_list_item_callback callback, void *cookie);
void *buf_list_payload(toy_buf_list *list);
const void *buf_list_payload_const(const toy_buf_list *list);
void buf_list_payload_set(toy_buf_list *list, void *buf, size_t buf_size);
void buf_list_payload_set_2(toy_buf_list *list, void *buf1, size_t buf1_size, void *buf2, size_t buf2_size);
#define buf_list_payload_typed(list, type) ((type *) buf_list_payload(list))
#define buf_list_payload_const_typed(list, type) ((const type *) buf_list_payload_const(list))

#endif /* TOY_BUF_LIST_H */
