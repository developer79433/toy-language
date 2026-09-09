#ifndef TOY_MAP_PTR_H
#define TOY_MAP_PTR_H 1

#include <stddef.h>

#include "map-ptr-types.h"
#include "buf-list.h"

void map_ptr_init(map_ptr *map);
map_ptr *map_ptr_alloc(void);
void *map_ptr_get(map_ptr *map, const void *key, size_t key_len);
const void *map_ptr_get_const(const map_ptr *map, const void *key, size_t key_len);
set_result map_ptr_set(map_ptr *map, const void *key, size_t key_len, void *ptr);
size_t map_ptr_size(const map_ptr *map);
delete_result map_ptr_delete(map_ptr *map, const void *key, size_t key_len);
void map_ptr_reset(map_ptr *map);
void map_ptr_free(map_ptr *map);
#ifdef NDEBUG
#define map_ptr_assert_valid(map) do {} while (0)
#else /* ndef NDEBUG */
void map_ptr_assert_valid(const map_ptr *map);
#endif /* ndef NDEBUG */

#endif /* TOY_MAP_PTR_H */
