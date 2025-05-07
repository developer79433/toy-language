#include <string.h>
#include <assert.h>

#include "str.h"
#include "map-ptr.h"
#include "map-buf.h"

map_ptr *map_ptr_alloc(void)
{
    return (map_ptr *) map_buf_alloc();
}

enumeration_result map_ptr_foreach(map_ptr *map, map_ptr_entry_callback callback, void *cookie)
{
    return map_buf_foreach((map_buf *) map, (map_buf_entry_callback) callback, cookie);
}

enumeration_result map_ptr_foreach_const(const map_ptr *map, const_map_ptr_entry_callback callback, void *cookie)
{
    return map_buf_foreach_const((map_buf *) map, (const_map_buf_entry_callback) callback, cookie);
}

size_t map_ptr_size(const map_ptr *map)
{
    return map_buf_size((const map_buf *) map);
}

static map_ptr_entry *map_ptr_get_entry(map_ptr *map, const toy_str key)
{
    return (map_ptr_entry *) map_buf_get_entry((map_buf *) map, key);
}

void *map_ptr_get(map_ptr *map, const toy_str key)
{
    map_ptr_entry *entry = map_ptr_get_entry(map, key);
    if (entry) {
        return entry->ptr;
    }
    return NULL;
}

const void *map_ptr_get_const(const map_ptr *map, const toy_str key)
{
    return map_buf_get_const((map_buf *) map, key);
}

set_result map_ptr_set(map_ptr *map, const toy_str key, void *ptr)
{
    return map_buf_set((map_buf *) map, key, &ptr, sizeof(ptr));
}

void map_ptr_assert_valid(const map_ptr *map)
{
    return map_buf_assert_valid((const map_buf *) map);
}

delete_result map_ptr_delete(map_ptr *map, const toy_str key)
{
    return map_buf_delete((map_buf *) map, key);
}

void map_ptr_reset(map_ptr *map)
{
    return map_buf_reset((map_buf *) map);
}

void map_ptr_free(map_ptr *map)
{
    return map_buf_free((map_buf *) map);
}
