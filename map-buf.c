#include <string.h>
#include <assert.h>

#include "map-buf.h"
#include "generic-map.h"
#include "buf-list.h"
#include "map-buf-entry-list.h"
#include "str.h"

map_buf_entry_list **map_buf_get_bucket(map_buf *map, const toy_str key)
{
    return (map_buf_entry_list **) generic_map_get_bucket((generic_map *) map, key);
}

void *map_buf_get_bucket_key(toy_buf_list *bucket, const toy_str key)
{
    return generic_map_get_bucket_key(bucket, key);
}

enumeration_result map_buf_foreach(map_buf *map, map_buf_entry_callback callback, void *cookie)
{
    return generic_map_foreach((generic_map *) map, (generic_map_entry_callback) callback, cookie);
}

enumeration_result map_buf_foreach_const(const map_buf *map, const_map_buf_entry_callback callback, void *cookie)
{
    return generic_map_foreach_const((generic_map *) map, (const_generic_map_entry_callback) callback, cookie);
}

int map_buf_set(map_buf *map, const toy_str key, void *buf, size_t buf_size)
{
    generic_map *gen_map = (generic_map *) map;
    map_buf_entry_list *new_entry;
    map_buf_entry_list **bucket = map_buf_get_bucket(map, key);
    if (*bucket) {
        for (map_buf_entry_list *entry = *bucket; entry; entry = entry->next) {
            map_buf_entry *map_entry = map_buf_entry_list_payload(entry);
            if (toy_str_equal(map_entry->key, key)) {
                /* Overwrite existing entry */
                map_buf_entry_list_payload_set_buf(entry, buf, buf_size);
                return 0;
            }
        }
        /* Prepend new entry to existing bucket */
        new_entry = map_buf_entry_list_alloc(key, buf, buf_size);
        new_entry->next = *bucket;
    } else {
        /* New entry in new bucket */
        new_entry = map_buf_entry_list_alloc(key, buf, buf_size);
    }
    *bucket = new_entry;
    gen_map->num_items++;
    return 1;
}

void map_buf_assert_valid(const map_buf *map)
{
    return generic_map_assert_valid((const generic_map *) map);
}

void map_buf_free(map_buf *map)
{
    return generic_map_free((generic_map *) map);
}

size_t map_buf_size(const map_buf *map)
{
    return generic_map_size((const generic_map *) map);
}

void map_buf_reset(map_buf *map)
{
    return generic_map_reset((generic_map *) map);
}

int map_buf_delete(map_buf *map, const toy_str key)
{
    return generic_map_delete((generic_map *) map, key);
}
