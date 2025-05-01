#ifndef TOY_STR_LIST_INLINE_TYPES_H
#define TOY_STR_LIST_INLINE_TYPES_H 1

#include "str-types.h"

typedef struct toy_str_list_inline_struct {
    struct toy_str_list_inline_struct *next;
    toy_char c; /* really variable-length, null-terminated string */
} toy_str_list_inline;

#endif /* TOY_STR_LIST_INLINE_TYPES_H */
