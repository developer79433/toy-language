#include <string.h>
#include <assert.h>

#include "map-val.h"
#include "str.h"
#include "val.h"
#include "map-buf.h"
#include "generic-map.h"

map_val *map_val_alloc(void)
{
    assert(offsetof(map_val, buckets) == offsetof(generic_map, buckets));
    assert(offsetof(map_val, num_items) == offsetof(generic_map, num_items));
    return (map_val *) map_buf_alloc();
}

int map_val_set(map_val *map, const toy_str key, toy_val *value)
{
    return map_buf_set((map_buf *) map, key, value, sizeof(*value));
}

toy_val *map_val_get(map_val *map, const toy_str key)
{
    return (toy_val *) map_buf_get((map_buf *) map, key);
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
