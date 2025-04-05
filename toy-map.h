#ifndef TOY_MAP_H
#define TOY_MAP_H 1

#include <stddef.h>
#include <stdio.h>

#include "toy-map-types.h"
#include "toy-str-types.h"
#include "toy-val-types.h"

toy_map *alloc_map();
toy_val *map_get(toy_map *map, const toy_str key);
int map_set(toy_map *map, const toy_str key, const toy_val *value);
int map_delete(toy_map *map, const toy_str key);
typedef void(*map_entry_callback)(void *cookie, const toy_str key, const toy_val *value);
void map_enum(const toy_map *map, map_entry_callback callback, void *cookie);
void dump_map(FILE *f, const toy_map *map);
size_t map_len(const toy_map *map);

#endif /* TOY_MAP_H */
