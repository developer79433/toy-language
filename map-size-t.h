#ifndef TOY_MAP_SIZE_T_H
#define TOY_MAP_SIZE_T_H 1

#include <stdio.h>
#include <stddef.h>

#include "str-types.h"
#include "iter-types.h"
#include "map-size-t-types.h"
#include "map-size-t-entry-list-types.h"

void map_size_t_init(map_size_t *map);
map_size_t *map_size_t_alloc(void);
void map_size_t_assert_valid(const map_size_t *map);
size_t *map_size_t_get(map_size_t *map, const toy_str key);
const size_t *map_size_t_get_const(const map_size_t *map, const toy_str key);
set_result map_size_t_set(map_size_t *map, const toy_str key, size_t value);
delete_result map_size_t_delete(map_size_t *map, const toy_str key);
enumeration_result map_size_t_foreach_const(const map_size_t *map, const_map_size_t_entry_callback callback, void *cookie);
enumeration_result map_size_t_foreach(map_size_t *map, map_size_t_entry_callback callback, void *cookie);
size_t map_size_t_size(const map_size_t *map);
void map_size_t_dump(FILE *f, const map_size_t *map);
void map_size_t_reset(map_size_t *map);
void map_size_t_free(map_size_t *map);

#endif /* TOY_MAP_SIZE_T_H */
