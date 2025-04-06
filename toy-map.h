#ifndef TOY_MAP_H
#define TOY_MAP_H 1

#include <stddef.h>
#include <stdio.h>

#include "toy-map-types.h"
#include "toy-str-types.h"
#include "toy-val-types.h"

toy_map *alloc_map();
void free_map(toy_map *map);
toy_val *map_get(toy_map *map, const toy_str key);
int map_set(toy_map *map, const toy_str key, const toy_val *value);
int map_delete(toy_map *map, const toy_str key);
void map_foreach(toy_map *map, map_entry_callback callback, void *cookie);
void map_foreach_const(const toy_map *map, const_map_entry_callback callback, void *cookie);
void dump_map(FILE *f, const toy_map *map);
void dump_map_keys(FILE *f, const toy_map *map);
size_t map_len(const toy_map *map);
void reset_map(toy_map *map);
void map_assert_valid(const toy_map *map);

#endif /* TOY_MAP_H */
