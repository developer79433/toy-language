#include <string.h>

#include "str.h"
#include "map-val.h"
#include "generic-map.h"

int map_val_set(generic_map *map, const toy_str key, const toy_val *value)
{
    generic_map_entry_list *new_entry;
    generic_map_entry_list **bucket = get_bucket(map, key);
    if (*bucket) {
        for (generic_map_entry_list *entry = *bucket; entry; entry = entry->next) {
            if (toy_str_equal(entry->entry.key, key)) {
                /* Overwrite existing entry */
                memcpy(&entry->entry.value, value, sizeof(*value));
                return 0;
            }
        }
        /* Prepend new entry to existing bucket */
        new_entry = map_val_entry_list_alloc(key, (toy_val *) value);
        new_entry->next = *bucket;
    } else {
        /* New entry in new bucket */
        new_entry = map_val_entry_list_alloc(key, (toy_val *) value);
    }
    *bucket = new_entry;
    map->num_items++;
    return 1;
}

toy_val *map_val_get(generic_map *map, const toy_str key)
{
    generic_map_entry_list **bucket = get_bucket(map, key);
    if (*bucket) {
        toy_val *existing_value = map_val_get_bucket_key(*bucket, key);
        if (existing_value) {
            return existing_value;
        }
        return NULL;
    }
    return NULL;
}
