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
        new_entry = generic_map_entry_list_alloc(key, (toy_val *) value);
        new_entry->next = *bucket;
    } else {
        /* New entry in new bucket */
        new_entry = generic_map_entry_list_alloc(key, (toy_val *) value);
    }
    *bucket = new_entry;
    map->num_items++;
    return 1;
}
