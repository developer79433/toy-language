#ifndef TOY_GENERIC_MAP_TYPES_H
#define TOY_GENERIC_MAP_TYPES_H 1

#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"

typedef struct generic_map_entry_struct {
    toy_str key;
    toy_val value;
} generic_map_entry;

typedef struct generic_map_entry_list_struct {
    struct generic_map_entry_list_struct *next;
    generic_map_entry entry;
} generic_map_entry_list;

typedef item_callback_result (*map_entry_callback)(void *cookie, toy_str key, toy_val *value);
typedef item_callback_result (*const_map_entry_callback)(void *cookie, const toy_str key, const toy_val *value);

#endif /* TOY_GENERIC_MAP_TYPES_H */
