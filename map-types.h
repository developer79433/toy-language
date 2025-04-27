#ifndef TOY_MAP_TYPES_H
#define TOY_MAP_TYPES_H 1

#include "str-types.h"
#include "val-types.h"

struct toy_map_struct;
typedef struct toy_map_struct toy_map;

typedef void(*map_entry_callback)(void *cookie, toy_str key, toy_val *value);
typedef void(*const_map_entry_callback)(void *cookie, const toy_str key, const toy_val *value);

#endif /* TOY_MAP_TYPES_H */
