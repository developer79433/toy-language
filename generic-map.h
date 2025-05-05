#ifndef TOY_GENERIC_MAP_H
#define TOY_GENERIC_MAP_H 1

#include <stddef.h>
#include <stdio.h>

#include "generic-map-types.h"
#include "str-types.h"

generic_map *generic_map_alloc(void);
toy_buf_list **generic_map_get_bucket(generic_map *map, toy_str key);
void *generic_map_bucket_get_key(toy_buf_list *bucket, const toy_str key);
void generic_map_free(generic_map *map);
int generic_map_delete(generic_map *map, const toy_str key);
enumeration_result generic_map_foreach(generic_map *map, generic_map_entry_callback callback, void *cookie);
enumeration_result generic_map_foreach_const(const generic_map *map, const_generic_map_entry_callback callback, void *cookie);
void generic_map_dump(FILE *f, const generic_map *map);
void generic_map_dump_keys(FILE *f, const generic_map *map);
size_t generic_map_size(const generic_map *map);
void generic_map_reset(generic_map *map);
void generic_map_assert_valid(const generic_map *map);

#endif /* TOY_GENERIC_MAP_H */
