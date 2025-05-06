#ifndef TOY_GENERIC_MAP_ENTRY_LIST_TYPES_H
#define TOY_GENERIC_MAP_ENTRY_LIST_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"
#include "str-types.h"

/* TODO: Delete me */
typedef struct generic_map_payload_struct {
} generic_map_payload;

typedef struct generic_map_entry_struct {
    toy_str key;
    generic_map_payload payload;
} generic_map_entry;

typedef struct generic_map_entry_list_struct {
    struct generic_map_entry_list_struct *next;
    generic_map_entry entry;
} generic_map_entry_list;

typedef item_callback_result (*generic_map_entry_list_item_callback)(void *cookie, size_t index, generic_map_entry_list *item);
typedef item_callback_result (*const_generic_map_entry_list_item_callback)(void *cookie, size_t index, const generic_map_entry_list *item);

#endif /* TOY_GENERIC_MAP_ENTRY_LIST_TYPES_H */
