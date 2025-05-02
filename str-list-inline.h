#ifndef TOY_STR_LIST_INLINE_H
#define TOY_STR_LIST_INLINE_H 1

#include "str-types.h"
#include "str-list-inline-types.h"

toy_str_list_inline *str_list_inline_alloc(const char *str);
size_t str_list_inline_len(const toy_str_list_inline *list);
toy_str_list_inline *str_list_inline_append(toy_str_list_inline *list, toy_str new_item);
iter_result str_list_inline_foreach(toy_str_list_inline *list, toy_str_list_inline_item_callback callback, void *cookie);
iter_result str_list_inline_foreach_const(const toy_str_list_inline *list, const_toy_str_list_inline_item_callback callback, void *cookie);
toy_str str_list_inline_payload(toy_str_list_inline *list);
const toy_str str_list_inline_payload_const(const toy_str_list_inline *list);
void str_list_inline_payload_set(const toy_str_list_inline *list, toy_str str);
void str_list_inline_free(toy_str_list_inline *list);

#endif /* TOY_STR_LIST_INLINE_H */
