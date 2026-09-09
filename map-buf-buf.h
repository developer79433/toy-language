#ifndef TOY_MAP_BUF_BUF_H
#define TOY_MAP_BUF_BUF_H 1

#include <stddef.h>

#include "map-buf-buf-types.h"
#include "map-buf-buf-entry-list-types.h"

void map_buf_buf_init(map_buf_buf *map);
map_buf_buf *map_buf_buf_alloc(void);
map_buf_buf_entry *map_buf_buf_get_entry(map_buf_buf *map, const void *key, size_t key_len);
const map_buf_buf_entry *map_buf_buf_get_entry_const(const map_buf_buf *map, const void *key, size_t key_len);
void *map_buf_buf_get(map_buf_buf *map, const void *key, size_t key_len);
const void *map_buf_buf_get_const(const map_buf_buf *map, const void *key, size_t key_len);
set_result map_buf_buf_set(map_buf_buf *map, const void *key, size_t key_len, const void *buf, size_t buf_size);
size_t map_buf_buf_size(const map_buf_buf *map);
set_result map_buf_buf_set(map_buf_buf *map, const void *key, size_t key_len, const void *buf, size_t buf_size);
delete_result map_buf_buf_delete(map_buf_buf *map, const void *key, size_t key_len);
#ifdef NDEBUG
#define map_buf_buf_assert_valid(map) do {} while (0)
#else /* ndef NDEBUG */
void map_buf_buf_assert_valid(const map_buf_buf *map);
#endif /* ndef NDEBUG */
void map_buf_buf_reset(map_buf_buf *map);
void map_buf_buf_free(map_buf_buf *map);

#endif /* TOY_MAP_BUF_BUF_H */
