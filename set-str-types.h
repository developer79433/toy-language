#ifndef TOY_SET_STR_TYPES_H
#define TOY_SET_STR_TYPES_H 1

#include "map-buf-buf-types.h"

typedef struct set_str_entry_struct {
    toy_str key;
    size_t key_len;
    int value; /* TODO: Delete me */
} set_str_entry;

typedef struct set_str_entry_list_struct {
    struct set_str_entry_list_struct *next;
    set_str_entry entry;
} set_str_entry_list;

typedef struct set_str_struct {
    map_buf_buf map;
} set_str;

#endif /* TOY_SET_STR_TYPES_H */
