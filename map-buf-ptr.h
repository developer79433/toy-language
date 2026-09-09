#ifndef TOY_MAP_BUF_PTR_H
#define TOY_MAP_BUF_PTR_H 1

#include <stddef.h>

#include "map-buf-ptr-types.h"
#include "buf-list.h"

void map_buf_ptr_init(map_buf_ptr *map);
map_buf_ptr *map_buf_ptr_alloc(void);
void *map_buf_ptr_get(map_buf_ptr *map, const void *key, size_t key_len);
const void *map_buf_ptr_get_const(const map_buf_ptr *map, const void *key, size_t key_len);
set_result map_buf_ptr_set(map_buf_ptr *map, const void *key, size_t key_len, void *ptr);
size_t map_buf_ptr_size(const map_buf_ptr *map);
delete_result map_buf_ptr_delete(map_buf_ptr *map, const void *key, size_t key_len);
void map_buf_ptr_reset(map_buf_ptr *map);
void map_buf_ptr_free(map_buf_ptr *map);
#ifdef NDEBUG
#define map_buf_ptr_assert_valid(map) do {} while (0)
#else /* ndef NDEBUG */
void map_buf_ptr_assert_valid(const map_buf_ptr *map);
#endif /* ndef NDEBUG */

#endif /* TOY_MAP_BUF_PTR_H */
