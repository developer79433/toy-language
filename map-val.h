#ifndef TOY_MAP_VAL_H
#define TOY_MAP_VAL_H 1

#include "val-types.h"
#include "generic-map-types.h"

toy_val *map_val_get(generic_map *map, const toy_str key);
int map_val_set(generic_map *map, const toy_str key, const toy_val *value);

#endif /* TOY_MAP_VAL_H */
