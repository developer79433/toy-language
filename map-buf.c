#include <string.h>
#include <assert.h>

#include "map-buf.h"
#include "generic-map.h"
#include "map-buf-entry-list.h"
#include "str.h"

map_buf *map_buf_alloc(void)
{
    assert(offsetof(map_buf, buckets) == offsetof(generic_map, buckets));
    assert(offsetof(map_buf, num_items) == offsetof(generic_map, num_items));
    return (map_buf *) generic_map_alloc();
}

static map_buf_entry_list **map_buf_get_bucket(map_buf *map, const toy_str key)
{
    assert(offsetof(map_buf, buckets) == offsetof(generic_map, buckets));
    assert(offsetof(map_buf, num_items) == offsetof(generic_map, num_items));
    return (map_buf_entry_list **) generic_map_get_bucket((generic_map *) map, key);
}

enumeration_result map_buf_foreach(map_buf *map, map_buf_entry_callback callback, void *cookie)
{
    return generic_map_foreach((generic_map *) map, (generic_map_entry_callback) callback, cookie);
}

enumeration_result map_buf_foreach_const(const map_buf *map, const_map_buf_entry_callback callback, void *cookie)
{
    return generic_map_foreach_const((generic_map *) map, (const_generic_map_entry_callback) callback, cookie);
}

map_buf_entry *map_buf_get_entry(map_buf *map, const toy_str key)
{
    return (map_buf_entry *) generic_map_get_entry((generic_map *) map, key);
}

const map_buf_entry *map_buf_get_entry_const(const map_buf *map, const toy_str key)
{
    return (const map_buf_entry *) generic_map_get_entry_const((const generic_map *) map, key);
}

void *map_buf_get(map_buf *map, const toy_str key)
{
    map_buf_entry *entry = map_buf_get_entry(map, key);
    if (entry) {
        return &entry->c;
    }
    return NULL;
}

const void *map_buf_get_const(const map_buf *map, const toy_str key)
{
    const map_buf_entry *entry = map_buf_get_entry_const(map, key);
    if (entry) {
        return &entry->c;
    }
    return NULL;
}

typedef struct map_buf_set_entry_cb_args_struct {
    toy_str desired_key;
    const void *new_value;
    size_t new_value_size;
} map_buf_set_entry_cb_args;

static item_callback_result map_buf_set_entry_callback(void *cookie, size_t index, map_buf_entry_list *list)
{
    map_buf_set_entry_cb_args *args = (map_buf_set_entry_cb_args *) cookie;
    map_buf_entry *entry = map_buf_entry_list_payload(list);
    if (toy_str_equal(entry->key, args->desired_key)) {
        /* Overwrite existing entry */
        map_buf_entry_list_payload_set(list, args->new_value, args->new_value_size);
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

set_result map_buf_set(map_buf *map, const toy_str key, const void *buf, size_t buf_size)
{
    map_buf_entry_list *new_entry;
    map_buf_entry_list **bucket = map_buf_get_bucket(map, key);
    if (*bucket) {
        map_buf_set_entry_cb_args map_buf_entry_args = { .desired_key = key, .new_value = buf, .new_value_size = buf_size };
        enumeration_result res = map_buf_entry_list_foreach(*bucket, map_buf_set_entry_callback, &map_buf_entry_args);
        if (res == ENUMERATION_INTERRUPTED) {
            return SET_EXISTING;
        }
        assert(ENUMERATION_COMPLETE == res);
        /* Prepend new entry to existing bucket */
        new_entry = map_buf_entry_list_alloc(key, buf, buf_size);
        new_entry->next = *bucket;
    } else {
        /* New entry in new bucket */
        new_entry = map_buf_entry_list_alloc(key, buf, buf_size);
    }
    *bucket = new_entry;
    map->num_items++;
    return SET_NEW;
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

delete_result map_buf_delete(map_buf *map, const toy_str key)
{
    return generic_map_delete((generic_map *) map, key);
}
