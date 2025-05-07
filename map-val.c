#include <string.h>
#include <assert.h>

#include "map-val.h"
#include "map-val-entry-list.h"
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

set_result map_val_set(map_val *map, const toy_str key, toy_val *value)
{
    return map_buf_set((map_buf *) map, key, value, sizeof(*value));
}

toy_val *map_val_get(map_val *map, const toy_str key)
{
    return (toy_val *) map_buf_get((map_buf *) map, key);
}

const toy_val *map_val_get_const(const map_val *map, const toy_str key)
{
    return (const toy_val *) map_buf_get_const((const map_buf *) map, key);
}

void map_val_free(map_val *map)
{
    return map_buf_free((map_buf *) map);
}

size_t map_val_size(map_val *map)
{
    return map_buf_size((map_buf *) map);
}

delete_result map_val_delete(map_val *map, const toy_str key)
{
    return map_buf_delete((map_buf *) map, key);
}

void map_val_reset(map_val *map)
{
    return map_buf_reset((map_buf *) map);
}

typedef struct dump_item_cb_args_struct {
    int output_anything;
    FILE *f;
} dump_item_cb_args;

static item_callback_result dump_item_callback(void *cookie, const map_val_entry *entry)
{
    dump_item_cb_args *args = (dump_item_cb_args *) cookie;
    if (args->output_anything) {
        fputs(", ", args->f);
    } else {
        fputc(' ', args->f);
    }
    map_val_entry_dump(entry, args->f);
    args->output_anything = 1;
    return CONTINUE_ENUMERATION;
}

void map_val_dump(FILE *f, const map_val *map)
{
    dump_item_cb_args cbargs = { .f = f, .output_anything = 0 };
    fputc('{', f);
    map_val_foreach_const(map, dump_item_callback, &cbargs);
    if (cbargs.output_anything) {
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
