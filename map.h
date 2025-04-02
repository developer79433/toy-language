#ifndef TOY_MAP_H
#define TPY_MAP_H 1

#include <stddef.h>

#include "ast.h"

toy_map *alloc_map();
toy_expr *map_get(toy_map *map, const toy_str key);
int map_set(toy_map *map, const toy_str key, toy_expr *value);
int map_set_expr(toy_map *map, const toy_expr *key, toy_expr *value);
int map_delete(toy_map *map, const toy_str key);
typedef void(*map_entry_callback)(void *cookie, const toy_str key, const toy_expr *value);
void map_enum(const toy_map *map, map_entry_callback callback, void *cookie);
void dump_map(FILE *f, const toy_map *map);
void dump_map_entry(FILE *f, const toy_map_entry *entry);
void dump_map_entries(FILE *f, const toy_map_entry *entries);
size_t map_len(const toy_map *map);

#endif /* TOY_MAP_H */
