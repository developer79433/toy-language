#ifndef TOY_MAP_PTR_H
#define TOY_MAP_PTR_H 1

#include "str-types.h"
#include "map-ptr-types.h"
#include "buf-list.h"

map_ptr *map_ptr_alloc(void);
void *map_ptr_get(map_ptr *map, const toy_str key);
int map_ptr_set(map_ptr *map, const toy_str key, void *ptr);

#endif /* TOY_MAP_PTR_H */
