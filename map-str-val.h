#ifndef TOY_MAP_STR_VAL_H
#define TOY_MAP_STR_VAL_H 1

#include "iter-types.h"
#include "map-str-val-types.h"
#include "val-types.h"
#include "map-str-val-entry-list-types.h"

void map_str_val_init(map_str_val *map);
map_str_val *map_str_val_alloc(void);
toy_bool map_str_val_equal(const map_str_val *map1, const map_str_val *map2);
toy_val *map_str_val_get(map_str_val *map, const toy_str key);
const toy_val *map_str_val_get_const(const map_str_val *map, const toy_str key);
set_result map_str_val_set(map_str_val *map, const toy_str key, const toy_val *value);
delete_result map_str_val_delete(map_str_val *map, const toy_str key);
size_t map_str_val_size(const map_str_val *map);
#ifdef NDEBUG
#define map_str_val_assert_valid(map) do {} while (0)
#else /* ndef NDEBUG */
void map_str_val_assert_valid(const map_str_val *map);
#endif /* ndef NDEBUG */
void map_str_val_dump(const map_str_val *map);
void map_str_val_reset(map_str_val *map);
void map_str_val_free(map_str_val *map);

#endif /* TOY_MAP_STR_VAL_H */
