#include <string.h>

#include "map-val.h"
#include "str.h"
#include "mymalloc.h"
#include "val.h"
#include "generic-map.h"
#include "map-buf.h"
#include "buf-list.h"
#include "ptr-list.h"
#include "map-buf-entry-list.h"

map_val *map_val_alloc(void)
{
    return (map_val *) generic_map_alloc();
}

int map_val_set(map_val *map, const toy_str key, toy_val *value)
{
    return map_buf_set((map_buf *) map, key, value, sizeof(*value));
}

toy_val *map_val_get(map_val *map, const toy_str key)
{
    map_val_entry_list **bucket = map_val_get_bucket(map, key);
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
    return map_buf_free((map_buf *) map);
}

size_t map_val_size(map_val *map)
{
    return map_buf_size((map_buf *) map);
}

int map_val_delete(map_val *map, const toy_str key)
{
    return map_buf_delete((map_buf *) map, key);
}

void map_val_reset(map_val *map)
{
    return map_buf_reset((map_buf *) map);
}

static void dump_map_val_entry(FILE *f, const map_val_entry *entry)
{
    dump_str(f, entry->key);
    fputs(": ", f);
    val_dump(f, &entry->payload.value);
}

void map_val_dump(FILE *f, const map_val *map)
{
    int output_anything = 0;

    fputc('{', f);
    for (map_val_entry_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (const map_val_entry_list *list = *bucket; list; list = list->next) {
                if (output_anything) {
                    fputs(", ", f);
                } else {
                    fputc(' ', f);
                }
                dump_map_val_entry(f, &list->entry);
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
    /* TODO: Validate values */
    return map_buf_assert_valid((map_buf *) map);
}

enumeration_result map_val_foreach(map_val *map, map_val_entry_callback callback, void *cookie)
{
    return map_buf_foreach((map_buf *) map, (map_buf_entry_callback) callback, cookie);
}

enumeration_result map_val_foreach_const(const map_val *map, const_map_val_entry_callback callback, void *cookie)
{
    return map_buf_foreach_const((map_buf *) map, (const_map_buf_entry_callback) callback, cookie);
}

toy_val *map_val_get_bucket_key(map_val_entry_list *bucket, const toy_str key)
{
    return (toy_val *) map_buf_get_bucket_key((toy_buf_list *) bucket, key);
}
