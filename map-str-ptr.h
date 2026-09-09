#ifndef TOY_MAP_STR_PTR_H
#define TOY_MAP_STR_PTR_H 1

#include "str-types.h"
#include "iter-types.h"
#include "map-str-ptr-types.h"

void map_str_ptr_init(map_str_ptr *map);
map_str_ptr *map_str_ptr_alloc(void);
void *map_str_ptr_get(map_str_ptr *map, const toy_str key);
const void *map_str_ptr_get_const(const map_str_ptr *map, const toy_str key);
set_result map_str_ptr_set(map_str_ptr *map, const toy_str key, void *ptr);
size_t map_str_ptr_size(const map_str_ptr *map);
delete_result map_str_ptr_delete(map_str_ptr *map, const toy_str key);
void map_str_ptr_reset(map_str_ptr *map);
void map_str_ptr_free(map_str_ptr *map);
#ifdef NDEBUG
#define map_str_ptr_assert_valid(map) do {} while (0)
#else /* ndef NDEBUG */
void map_str_ptr_assert_valid(const map_str_ptr *map);
#endif /* ndef NDEBUG */

#endif /* TOY_MAP_STR_PTR_H */
