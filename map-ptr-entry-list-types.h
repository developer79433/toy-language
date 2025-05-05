#ifndef TOY_MAP_PTR_ENTRY_LIST_TYPES_H
#define TOY_MAP_PTR_ENTRY_LIST_TYPES_H 1

#include "iter-types.h"
#include "str-types.h"

typedef struct map_ptr_payload_struct {
    void *ptr;
} map_ptr_payload;

typedef struct map_ptr_entry_struct {
    toy_str key;
    map_ptr_payload payload;
} map_ptr_entry;

/* TODO: Goes in separate header */
typedef struct map_ptr_entry_list_struct {
    struct map_ptr_entry_list_struct *next;
    map_ptr_entry entry;
} map_ptr_entry_list;

typedef item_callback_result (*map_ptr_entry_list_item_callback)(void *cookie, size_t index, map_ptr_entry_list *item);
typedef item_callback_result (*const_map_ptr_entry_list_item_callback)(void *cookie, size_t index, const map_ptr_entry_list *item);

#endif /* TOY_MAP_PTR_ENTRY_LIST_TYPES_H */
