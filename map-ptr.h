#ifndef TOY_MAP_PTR_H
#define TOY_MAP_PTR_H 1

#include <stddef.h>

#include "str-types.h"
#include "map-ptr-types.h"
#include "buf-list.h"

map_ptr *map_ptr_alloc(void);
void *map_ptr_get(map_ptr *map, const toy_str key);
const void *map_ptr_get_const(const map_ptr *map, const toy_str key);
set_result map_ptr_set(map_ptr *map, const toy_str key, void *ptr);
size_t map_ptr_size(const map_ptr *map);
delete_result map_ptr_delete(map_ptr *map, const toy_str key);
enumeration_result map_ptr_foreach(map_ptr *map, map_ptr_entry_callback callback, void *cookie);
enumeration_result map_ptr_foreach_const(const map_ptr *map, const_map_ptr_entry_callback callback, void *cookie);
void map_ptr_reset(map_ptr *map);
void map_ptr_free(map_ptr *map);

#endif /* TOY_MAP_PTR_H */
