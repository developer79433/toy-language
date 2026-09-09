#include <string.h>
#include <assert.h>

#include "map-buf.h"
#include "generic-map.h"
#include "map-buf-entry-list.h"
#include "list-visitor.h"

void map_buf_init(map_buf *map)
{
    assert(offsetof(map_buf, buckets) == offsetof(generic_map, buckets));
    assert(offsetof(map_buf, num_items) == offsetof(generic_map, num_items));
    return map_init((generic_map *) map);
}

map_buf *map_buf_alloc(void)
{
    assert(offsetof(map_buf, buckets) == offsetof(generic_map, buckets));
    assert(offsetof(map_buf, num_items) == offsetof(generic_map, num_items));
    return (map_buf *) map_alloc();
}

static map_buf_entry_list **map_buf_get_bucket(map_buf *map, const void *key, size_t key_len)
{
    assert(offsetof(map_buf, buckets) == offsetof(generic_map, buckets));
    assert(offsetof(map_buf, num_items) == offsetof(generic_map, num_items));
    return (map_buf_entry_list **) map_get_bucket_ptr((generic_map *) map, key, key_len);
}

map_buf_entry *map_buf_get_entry(map_buf *map, const void *key, size_t key_len)
{
    return (map_buf_entry *) map_get_entry((generic_map *) map, key, key_len);
}

const map_buf_entry *map_buf_get_entry_const(const map_buf *map, const void *key, size_t key_len)
{
    return (const map_buf_entry *) map_get_entry_const((const generic_map *) map, key, key_len);
}

void *map_buf_get(map_buf *map, const void *key, size_t key_len)
{
    map_buf_entry *entry = map_buf_get_entry(map, key, key_len);
    if (entry) {
        return &entry->c;
    }
    return NULL;
}

const void *map_buf_get_const(const map_buf *map, const void *key, size_t key_len)
{
    const map_buf_entry *entry = map_buf_get_entry_const(map, key, key_len);
    if (entry) {
        return &entry->c;
    }
    return NULL;
}

typedef struct map_buf_set_visitor_struct {
    list_visitor list_vis;
    void *desired_key;
    size_t desired_key_len;
    const void *new_value;
    size_t new_value_size;
} map_buf_set_visitor;

static item_callback_result map_buf_set_entry_callback(map_buf_set_visitor *set_vis, size_t index, map_buf_entry_list *list)
{
    map_buf_entry *entry = map_buf_entry_list_payload(list);
    if (
        (entry->key_len == set_vis->desired_key_len) &&
        (0 == memcmp(entry->key, set_vis->desired_key, entry->key_len))
    ) {
        /* Overwrite existing entry */
        map_buf_entry_list_payload_set(list, set_vis->new_value, set_vis->new_value_size);
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

set_result map_buf_set(map_buf *map, const void *key, size_t key_len, const void *buf, size_t buf_size)
{
    map_buf_entry_list *new_entry;
    map_buf_entry_list **bucket = map_buf_get_bucket(map, key, key_len);
    if (*bucket) {
        map_buf_set_visitor set_vis = {
            .list_vis.visit_entry = (list_entry_visit_func) map_buf_set_entry_callback,
            .desired_key = (void *) key,
            .desired_key_len = key_len,
            .new_value = buf,
            .new_value_size = buf_size
        };
        enumeration_result res = list_visitor_visit_list((list_visitor *) &set_vis, (generic_list *) *bucket);
        if (res == ENUMERATION_INTERRUPTED) {
            return SET_EXISTING;
        }
        assert(ENUMERATION_COMPLETE == res);
        /* Prepend new entry to existing bucket */
        new_entry = map_buf_entry_list_alloc(key, key_len, buf, buf_size);
        new_entry->next = *bucket;
    } else {
        /* New entry in new bucket */
        new_entry = map_buf_entry_list_alloc(key, key_len, buf, buf_size);
    }
    *bucket = new_entry;
    map->num_items++;
    return SET_NEW;
}

#ifndef NDEBUG
void map_buf_assert_valid(const map_buf *map)
{
    return map_assert_valid((const generic_map *) map);
}
#endif /* ndef NDEBUG */

void map_buf_free(map_buf *map)
{
    return map_free((generic_map *) map);
}

size_t map_buf_size(const map_buf *map)
{
    return map_size((const generic_map *) map);
}

void map_buf_reset(map_buf *map)
{
    return map_reset((generic_map *) map);
}

delete_result map_buf_delete(map_buf *map, const void *key, size_t key_len)
{
    return map_delete((generic_map *) map, key, key_len);
}
