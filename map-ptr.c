#include <string.h>

#include "str.h"
#include "map-ptr.h"
#include "map-ptr-entry-list.h"
#include "mymalloc.h"
#include "generic-map.h"

map_ptr *map_ptr_alloc(void)
{
    return (map_ptr *) generic_map_alloc();
}

map_ptr_entry_list **map_ptr_get_bucket(map_ptr *map, const toy_str key)
{
    return (map_ptr_entry_list **) generic_map_get_bucket((generic_map *) map, key);
}

void *map_ptr_get_bucket_key(toy_buf_list *bucket, const toy_str key)
{
    return generic_map_bucket_get_key(bucket, key);
}

enumeration_result map_ptr_foreach(map_ptr *map, map_ptr_entry_callback callback, void *cookie)
{
    return generic_map_foreach((generic_map *) map, (generic_map_entry_callback) callback, cookie);
}

enumeration_result map_ptr_foreach_const(const map_ptr *map, const_map_ptr_entry_callback callback, void *cookie)
{
    return generic_map_foreach_const((generic_map *) map, (const_generic_map_entry_callback) callback, cookie);
}

int map_ptr_set(map_ptr *map, const toy_str key, void *ptr)
{
    generic_map *gen_map = (generic_map *) map;
    map_ptr_entry_list *new_entry;
    map_ptr_entry_list **bucket = map_ptr_get_bucket(map, key);
    if (*bucket) {
        /* TODO: Use map_ptr_entry_list_foreach */
        for (map_ptr_entry_list *list = *bucket; list; list = list->next) {
            map_ptr_entry *map_entry = map_ptr_entry_list_payload(list);
            if (toy_str_equal(map_entry->key, key)) {
                /* Overwrite existing entry */
                map_ptr_entry_list_payload_set(list, ptr);
                return 0;
            }
        }
        /* Prepend new entry to existing bucket */
        new_entry = map_ptr_entry_list_alloc(key, ptr);
        new_entry->next = *bucket;
    } else {
        /* New entry in new bucket */
        new_entry = map_ptr_entry_list_alloc(key, ptr);
    }
    *bucket = new_entry;
    gen_map->num_items++;
    return 1;
}

void map_ptr_assert_valid(const map_ptr *map)
{
    return generic_map_assert_valid((const generic_map *) map);
}
