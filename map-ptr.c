#include <string.h>
#include <assert.h>

#include "str.h"
#include "map-ptr.h"
#include "map-buf.h"

void map_ptr_init(map_ptr *map)
{
    return map_buf_init((map_buf *) map);
}

map_ptr *map_ptr_alloc(void)
{
    return (map_ptr *) map_buf_alloc();
}

size_t map_ptr_size(const map_ptr *map)
{
    return map_buf_size((const map_buf *) map);
}

static map_ptr_entry *map_ptr_get_entry(map_ptr *map, const void *key, size_t key_len)
{
    return (map_ptr_entry *) map_buf_get_entry((map_buf *) map, key, key_len);
}

void *map_ptr_get(map_ptr *map, const void *key, size_t key_len)
{
    map_ptr_entry *entry = map_ptr_get_entry(map, key, key_len);
    if (entry) {
        return entry->ptr;
    }
    return NULL;
}

const void *map_ptr_get_const(const map_ptr *map, const void *key, size_t key_len)
{
    return map_buf_get_const((map_buf *) map, key, key_len);
}

set_result map_ptr_set(map_ptr *map, const void *key, size_t key_len, void *ptr)
{
    return map_buf_set((map_buf *) map, key, key_len, &ptr, sizeof(ptr));
}

#ifndef NDEBUG
void map_ptr_assert_valid(const map_ptr *map)
{
    return map_buf_assert_valid((const map_buf *) map);
}
#endif /* ndef NDEBUG */

delete_result map_ptr_delete(map_ptr *map, const void *key, size_t key_len)
{
    return map_buf_delete((map_buf *) map, key, key_len);
}

void map_ptr_reset(map_ptr *map)
{
    return map_buf_reset((map_buf *) map);
}

void map_ptr_free(map_ptr *map)
{
    return map_buf_free((map_buf *) map);
}
