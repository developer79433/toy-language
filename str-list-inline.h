#ifndef TOY_STR_LIST_INLINE_H
#define TOY_STR_LIST_INLINE_H 1

#include "str-types.h"
#include "str-list-inline-types.h"

toy_str_list_inline *str_list_inline_alloc(const char *str);
size_t str_list_inline_len(const toy_str_list_inline *list);
toy_str_list_inline *str_list_inline_append(toy_str_list_inline *list, toy_str new_item);
void str_list_inline_free(toy_str_list_inline *list);

#endif /* TOY_STR_LIST_INLINE_H */
