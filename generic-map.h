#ifndef TOY_GENERIC_MAP_H
#define TOY_GENERIC_MAP_H 1

#include <stddef.h>
#include <stdio.h>

#include "generic-map-types.h"
#include "str-types.h"
#include "val-types.h"

generic_map *generic_map_alloc(void);
void generic_map_free(generic_map *map);
toy_val *map_val_get(generic_map *map, const toy_str key);
int map_val_set(generic_map *map, const toy_str key, const toy_val *value);
int generic_map_delete(generic_map *map, const toy_str key);
enumeration_result generic_map_foreach(generic_map *map, map_entry_callback callback, void *cookie);
enumeration_result generic_map_foreach_const(const generic_map *map, const_map_entry_callback callback, void *cookie);
void generic_map_dump(FILE *f, const generic_map *map);
void generic_map_dump_keys(FILE *f, const generic_map *map);
size_t map_len(const generic_map *map);
void generic_map_reset(generic_map *map);
void generic_map_assert_valid(const generic_map *map);

#endif /* TOY_GENERIC_MAP_H */
