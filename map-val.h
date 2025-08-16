#ifndef TOY_MAP_VAL_H
#define TOY_MAP_VAL_H 1

#include "map-val-types.h"
#include "val-types.h"
#include "map-val-entry-list-types.h"

void map_val_init(map_val *map);
map_val *map_val_alloc(void);
map_val *map_val_dup(const map_val *map);
toy_val *map_val_get(map_val *map, const toy_str key);
const toy_val *map_val_get_const(const map_val *map, const toy_str key);
set_result map_val_set(map_val *map, const toy_str key, const toy_val *value);
delete_result map_val_delete(map_val *map, const toy_str key);
enumeration_result map_val_foreach_const(const map_val *map, const_map_val_entry_callback callback, void *cookie);
enumeration_result map_val_foreach(map_val *map, map_val_entry_callback callback, void *cookie);
size_t map_val_size(const map_val *map);
#ifdef NDEBUG
#define map_val_assert_valid(map) do {} while (0)
#else /* ndef NDEBUG */
void map_val_assert_valid(const map_val *map);
#endif /* ndef NDEBUG */
void map_val_dump(const map_val *map);
void map_val_reset(map_val *map);
void map_val_free(map_val *map);

#endif /* TOY_MAP_VAL_H */
