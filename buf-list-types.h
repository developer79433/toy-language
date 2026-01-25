#ifndef TOY_BUF_LIST_TYPES_H
#define TOY_BUF_LIST_TYPES_H 1

#include <stddef.h>
#include <inttypes.h>

#include "generic-list.h"

typedef struct toy_buf_list_struct {
    struct toy_buf_list_struct *next;
    /* TODO: Keep track of current length:
    size_t buf_size
    */
    uint8_t c; /* Really variable-length */
} toy_buf_list;

#endif /* TOY_BUF_LIST_TYPES_H */
