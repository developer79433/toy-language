#include <string.h>

#include "map-str-ptr.h"
#include "map-buf-ptr.h"

void map_str_ptr_init(map_str_ptr *map)
{
    map_buf_ptr_init((map_buf_ptr *) map);
}

map_str_ptr *map_str_ptr_alloc(void)
{
    return (map_str_ptr *) map_buf_ptr_alloc();
}

void *map_str_ptr_get(map_str_ptr *map, const toy_str key)
{
    return map_buf_ptr_get((map_buf_ptr *) map, key, strlen(key) + 1);
}

const void *map_str_ptr_get_const(const map_str_ptr *map, const toy_str key)
{
    return map_buf_ptr_get_const((const map_buf_ptr *) map, key, strlen(key) + 1);
}

set_result map_str_ptr_set(map_str_ptr *map, const toy_str key, void *ptr)
{
    return map_buf_ptr_set((map_buf_ptr *) map, key, strlen(key) + 1, ptr);
}

size_t map_str_ptr_size(const map_str_ptr *map)
{
    return map_buf_ptr_size((const map_buf_ptr *) map);
}

delete_result map_str_ptr_delete(map_str_ptr *map, const toy_str key)
{
    return map_buf_ptr_delete((map_buf_ptr *) map, key, strlen(key) + 1);
}

void map_str_ptr_reset(map_str_ptr *map)
{
    map_buf_ptr_reset((map_buf_ptr *) map);
}

void map_str_ptr_free(map_str_ptr *map)
{
    map_buf_ptr_free((map_buf_ptr *) map);
}

#ifdef NDEBUG
void map_str_ptr_assert_valid(const map_str_ptr *map)
{
    map_buf_ptr_assert_valid((const map_buf_ptr *) map);
}
#endif /* ndef NDEBUG */
