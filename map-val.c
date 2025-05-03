#include <string.h>

#include "str.h"
#include "val.h"
#include "map-val.h"
#include "generic-map.h"

map_val *map_val_alloc()
{
    return (map_val *) generic_map_alloc();
}

int map_val_set(map_val *map, const toy_str key, const toy_val *value)
{
    generic_map_entry_list *new_entry;
    generic_map_entry_list **bucket = get_bucket((generic_map *) map, key);
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

toy_val *map_val_get(map_val *map, const toy_str key)
{
    generic_map_entry_list **bucket = get_bucket((generic_map *) map, key);
    if (*bucket) {
        toy_val *existing_value = map_val_get_bucket_key(*bucket, key);
        if (existing_value) {
            return existing_value;
        }
        return NULL;
    }
    return NULL;
}

void map_val_free(map_val *map)
{
    return generic_map_free((generic_map *) map);
}

size_t map_val_size(map_val *map)
{
    return generic_map_size((generic_map *) map);
}

int map_val_delete(map_val *map, const toy_str key)
{
    return generic_map_delete((generic_map *) map, key);
}

void map_val_reset(map_val *map)
{
    return generic_map_reset((generic_map *) map);
}

static void dump_map_val_entry(FILE *f, const map_val_entry_list *entry)
{
    dump_str(f, entry->entry.key);
    fputs(": ", f);
    val_dump(f, &entry->entry.value);
}

void map_val_dump(FILE *f, const map_val *map)
{
    int output_anything = 0;

    fputc('{', f);
    for (map_val_entry_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (map_val_entry_list *entry = *bucket; entry; entry = entry->next) {
                if (output_anything) {
                    fputs(", ", f);
                } else {
                    fputc(' ', f);
                }
                dump_map_val_entry(f, entry);
                output_anything = 1;
            }
        }
    }
    if (output_anything) {
        fputc(' ', f);
    }
    fputc('}', f);
}

void map_val_assert_valid(map_val *map)
{
    return generic_map_assert_valid((generic_map *) map);
}

enumeration_result map_val_foreach(map_val *map, map_val_entry_callback callback, void *cookie)
{
    return generic_map_foreach((generic_map *) map, (generic_map_entry_callback) callback, cookie);
}

enumeration_result map_val_foreach_const(const map_val *map, const_map_val_entry_callback callback, void *cookie)
{
    return generic_map_foreach_const((generic_map *) map, (const_generic_map_entry_callback) callback, cookie);
}
