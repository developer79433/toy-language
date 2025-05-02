#ifndef TOY_STR_LIST_INLINE_TYPES_H
#define TOY_STR_LIST_INLINE_TYPES_H 1

#include "str-types.h"

typedef struct toy_str_list_inline_struct {
    struct toy_str_list_inline_struct *next;
    toy_char c; /* really variable-length, null-terminated string */
} toy_str_list_inline;

typedef item_callback_result (*toy_str_list_inline_item_callback)(void *cookie, size_t index, toy_str_list_inline *item);
typedef item_callback_result (*const_toy_str_list_inline_item_callback)(void *cookie, size_t index, const toy_str_list_inline *item);

#endif /* TOY_STR_LIST_INLINE_TYPES_H */
