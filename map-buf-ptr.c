#include <string.h>
#include <assert.h>

#include "map-buf-ptr.h"
#include "map-buf-buf.h"

void map_buf_ptr_init(map_buf_ptr *map)
{
    return map_buf_buf_init((map_buf_buf *) map);
}

map_buf_ptr *map_buf_ptr_alloc(void)
{
    return (map_buf_ptr *) map_buf_buf_alloc();
}

size_t map_buf_ptr_size(const map_buf_ptr *map)
{
    return map_buf_buf_size((const map_buf_buf *) map);
}

static map_buf_ptr_entry *map_buf_ptr_get_entry(map_buf_ptr *map, const void *key, size_t key_len)
{
    return (map_buf_ptr_entry *) map_buf_buf_get_entry((map_buf_buf *) map, key, key_len);
}

void *map_buf_ptr_get(map_buf_ptr *map, const void *key, size_t key_len)
{
    map_buf_ptr_entry *entry = map_buf_ptr_get_entry(map, key, key_len);
    if (entry) {
        return entry->ptr;
    }
    return NULL;
}

const void *map_buf_ptr_get_const(const map_buf_ptr *map, const void *key, size_t key_len)
{
    return map_buf_buf_get_const((map_buf_buf *) map, key, key_len);
}

set_result map_buf_ptr_set(map_buf_ptr *map, const void *key, size_t key_len, void *ptr)
{
    return map_buf_buf_set((map_buf_buf *) map, key, key_len, &ptr, sizeof(ptr));
}

#ifndef NDEBUG
void map_buf_ptr_assert_valid(const map_buf_ptr *map)
{
    return map_buf_buf_assert_valid((const map_buf_buf *) map);
}
#endif /* ndef NDEBUG */

delete_result map_buf_ptr_delete(map_buf_ptr *map, const void *key, size_t key_len)
{
    return map_buf_buf_delete((map_buf_buf *) map, key, key_len);
}

void map_buf_ptr_reset(map_buf_ptr *map)
{
    return map_buf_buf_reset((map_buf_buf *) map);
}

void map_buf_ptr_free(map_buf_ptr *map)
{
    return map_buf_buf_free((map_buf_buf *) map);
}
