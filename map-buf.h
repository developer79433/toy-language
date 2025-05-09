#ifndef TOY_MAP_BUF_H
#define TOY_MAP_BUF_H 1

#include <stddef.h>

#include "str-types.h"
#include "map-buf-types.h"
#include "map-buf-entry-list-types.h"

map_buf *map_buf_alloc(void);
map_buf_entry *map_buf_get_entry(map_buf *map, const toy_str key);
const map_buf_entry *map_buf_get_entry_const(const map_buf *map, const toy_str key);
void *map_buf_get(map_buf *map, const toy_str key);
const void *map_buf_get_const(const map_buf *map, const toy_str key);
set_result map_buf_set(map_buf *map, const toy_str key, const void *buf, size_t buf_size);
size_t map_buf_size(const map_buf *map);
set_result map_buf_set(map_buf *map, const toy_str key, const void *buf, size_t buf_size);
delete_result map_buf_delete(map_buf *map, const toy_str key);
enumeration_result map_buf_foreach(map_buf *map, map_buf_entry_callback callback, void *cookie);
enumeration_result map_buf_foreach_const(const map_buf *map, const_map_buf_entry_callback callback, void *cookie);
void map_buf_assert_valid(const map_buf *map);
void map_buf_reset(map_buf *map);
void map_buf_free(map_buf *map);

#endif /* TOY_MAP_BUF_H */
