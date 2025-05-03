#ifndef TOY_GENERIC_MAP_TYPES_H
#define TOY_GENERIC_MAP_TYPES_H 1

#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"

struct toy_map_struct;
typedef struct toy_map_struct toy_map;

typedef item_callback_result (*map_entry_callback)(void *cookie, toy_str key, toy_val *value);
typedef item_callback_result (*const_map_entry_callback)(void *cookie, const toy_str key, const toy_val *value);

#endif /* TOY_GENERIC_MAP_TYPES_H */
