#include <string.h>
#include <assert.h>

#include "str.h"
#include "map-ptr.h"
#include "map-ptr-entry-list.h"
#include "map-buf.h"
#include "mymalloc.h"

map_ptr *map_ptr_alloc(void)
{
    return (map_ptr *) map_buf_alloc();
}

static map_ptr_entry_list **map_ptr_get_bucket(map_ptr *map, const toy_str key)
{
    assert(offsetof(map_ptr, buckets) == offsetof(map_buf, buckets));
    assert(offsetof(map_ptr, num_items) == offsetof(map_buf, num_items));
    return (map_ptr_entry_list **) map_buf_get_bucket((map_buf *) map, key);
}

static map_ptr_entry *map_ptr_bucket_get_key(map_ptr_entry_list *bucket, const toy_str key)
{
    return (map_ptr_entry *) map_buf_bucket_get_key((map_buf_entry_list *) bucket, key);
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

void *map_ptr_get(map_ptr *map, const toy_str key)
{
    /* TODO */
    void **ptr = map_buf_get((map_buf *) map, key);
    // return *ptr;
    map_ptr_entry_list **bucket = map_ptr_get_bucket(map, key);
    if (*bucket) {
        map_ptr_entry *existing_entry = map_ptr_bucket_get_key(*bucket, key);
        if (existing_entry) {
            assert(toy_str_equal(existing_entry->key, key));
            assert(*ptr == existing_entry->ptr || ptr == existing_entry->ptr);
            assert(*ptr == existing_entry->ptr);
            return existing_entry->ptr;
        }
        return NULL;
    }
    return NULL;
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
