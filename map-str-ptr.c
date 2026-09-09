#include <string.h>

#include "map-str-ptr.h"
#include "map-ptr.h"

void map_str_ptr_init(map_str_ptr *map)
{
    map_ptr_init((map_ptr *) map);
}

map_str_ptr *map_str_ptr_alloc(void)
{
    return (map_str_ptr *) map_ptr_alloc();
}

void *map_str_ptr_get(map_str_ptr *map, const toy_str key)
{
    return map_ptr_get((map_ptr *) map, key, strlen(key) + 1);
}

const void *map_str_ptr_get_const(const map_str_ptr *map, const toy_str key)
{
    return map_ptr_get_const((const map_ptr *) map, key, strlen(key) + 1);
}

set_result map_str_ptr_set(map_str_ptr *map, const toy_str key, void *ptr)
{
    return map_ptr_set((map_ptr *) map, key, strlen(key) + 1, ptr);
}

size_t map_str_ptr_size(const map_str_ptr *map)
{
    return map_ptr_size((const map_ptr *) map);
}

delete_result map_str_ptr_delete(map_str_ptr *map, const toy_str key)
{
    return map_ptr_delete((map_ptr *) map, key, strlen(key) + 1);
}

void map_str_ptr_reset(map_str_ptr *map)
{
    map_ptr_reset((map_ptr *) map);
}

void map_str_ptr_free(map_str_ptr *map)
{
    map_ptr_free((map_ptr *) map);
}

#ifdef NDEBUG
void map_str_ptr_assert_valid(const map_str_ptr *map)
{
    map_ptr_assert_valid((const map_ptr *) map);
}
#endif /* ndef NDEBUG */
