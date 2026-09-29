#ifndef TOY_SET_BUF_TYPES_H
#define TOY_SET_BUF_TYPES_H 1

#include "map-buf-buf-types.h"

typedef struct set_buf_entry_struct {
    void *key;
    size_t key_len;
    int value; /* TODO: Delete me */
} set_buf_entry;

typedef struct set_buf_entry_list_struct {
    struct set_buf_entry_list_struct *next;
    set_buf_entry entry;
} set_buf_entry_list;

typedef struct set_buf_struct {
    map_buf_buf map;
} set_buf;

#endif /* TOY_SET_BUF_TYPES_H */
