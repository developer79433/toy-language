#ifndef TOY_STR_LIST_TYPES_H
#define TOY_STR_LIST_TYPES_H 1

#include <stddef.h>

#include "str-types.h"
#include "generic-list-types.h"

typedef struct toy_str_list_struct {
    struct toy_str_list_struct *next;
    toy_str str;
} toy_str_list;

#endif /* TOY_STR_LIST_TYPES_H */
