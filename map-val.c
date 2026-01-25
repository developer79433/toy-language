#include <string.h>
#include <assert.h>

#include "map-val.h"
#include "map-val-entry-list.h"
#include "str.h"
#include "val.h"
#include "map-buf.h"
#include "generic-map.h"
#include "debug.h"
#include "log.h"
#include "map-visitor.h"

void map_val_init(map_val *map)
{
    assert(offsetof(map_val, buckets) == offsetof(generic_map, buckets));
    assert(offsetof(map_val, num_items) == offsetof(generic_map, num_items));
    return map_buf_init((map_buf *) map);
}

map_val *map_val_alloc(void)
{
    assert(offsetof(map_val, buckets) == offsetof(generic_map, buckets));
    assert(offsetof(map_val, num_items) == offsetof(generic_map, num_items));
    return (map_val *) map_buf_alloc();
}

set_result map_val_set(map_val *map, const toy_str key, const toy_val *value)
{
    map_val_assert_valid(map);
    str_assert_valid(key);
    val_assert_valid(value);
    return map_buf_set((map_buf *) map, key, value, sizeof(*value));
}

toy_val *map_val_get(map_val *map, const toy_str key)
{
    map_val_assert_valid(map);
    str_assert_valid(key);
    toy_val *value = (toy_val *) map_buf_get((map_buf *) map, key);
    val_assert_valid_or_null(value);
    return value;
}

const toy_val *map_val_get_const(const map_val *map, const toy_str key)
{
    const toy_val *value = (const toy_val *) map_buf_get_const((const map_buf *) map, key);
    val_assert_valid_or_null(value);
    return value;
}

void map_val_free(map_val *map)
{
    return map_buf_free((map_buf *) map);
}

size_t map_val_size(const map_val *map)
{
    return map_buf_size((const map_buf *) map);
}

delete_result map_val_delete(map_val *map, const toy_str key)
{
    return map_buf_delete((map_buf *) map, key);
}

void map_val_reset(map_val *map)
{
    return map_buf_reset((map_buf *) map);
}

typedef struct map_val_dump_visitor_struct {
    const_map_visitor map_vis;
    int output_anything;
} map_val_dump_visitor;

static item_callback_result dump_item_callback(map_val_dump_visitor *dump_vis, const map_val_entry *entry)
{
    if (dump_vis->output_anything) {
        log_puts(", ");
    } else {
        log_putc(' ');
    }
    map_val_entry_dump(entry);
    dump_vis->output_anything = 1;
    return CONTINUE_ENUMERATION;
}

void map_val_dump(const map_val *map)
{
    map_val_dump_visitor dump_vis = { .map_vis.visit_entry = (const_map_entry_visit_func) dump_item_callback, .output_anything = 0 };
    log_putc('{');
    enumeration_result res = const_map_visitor_visit_map((const_map_visitor *) &dump_vis, (const generic_map *) map);
    assert(res == ENUMERATION_COMPLETE);
    if (dump_vis.output_anything) {
        log_putc(' ');
    }
    log_putc('}');
}

#ifndef NDEBUG

static item_callback_result item_assert_valid_callback(const_map_visitor *map_vis, const map_val_entry *entry)
{
    str_assert_valid(entry->key);
    if (valid_check_depth < VALID_CHECK_RECURSION_DEPTH) {
        valid_check_depth++;
        val_assert_valid(&entry->value);
        valid_check_depth--;
    }
    return CONTINUE_ENUMERATION;
}

void map_val_assert_valid(const map_val *map)
{
    const_map_visitor map_vis = { .visit_entry = (const_map_entry_visit_func) item_assert_valid_callback };
    enumeration_result res = const_map_visitor_visit_map(&map_vis, (const generic_map *) map);
    assert(res == ENUMERATION_COMPLETE);
    return map_buf_assert_valid((const map_buf *) map);
}
#endif /* ndef NDEBUG */

typedef struct compare_visitor_struct {
    const_map_visitor map_vis;
    const map_val *other_map;
} compare_visitor;

static item_callback_result test_entry_othermap_callback(compare_visitor *comp_vis, const map_val_entry *entry)
{
    assert(comp_vis->other_map != NULL);
    map_val_assert_valid(comp_vis->other_map);
    const toy_val *other_map_val = map_val_get_const(comp_vis->other_map, entry->key);
    if (other_map_val == NULL) {
        return STOP_ENUMERATION;
    }
    if (vals_equal(&entry->value, other_map_val)) {
        return CONTINUE_ENUMERATION;
    }
    return STOP_ENUMERATION;
}

toy_bool map_val_equal(const map_val *map1, const map_val *map2)
{
    if (map1 == map2) {
        return TOY_TRUE;
    }
    if (map_val_size(map1) != map_val_size(map2)) {
        return TOY_FALSE;
    }
    compare_visitor comp_vis = { .map_vis.visit_entry = (const_map_entry_visit_func) test_entry_othermap_callback, .other_map = map2 };
    enumeration_result res = const_map_visitor_visit_map((const_map_visitor *) &comp_vis, (const generic_map *) map1);
    if (res == ENUMERATION_COMPLETE) {
        return TOY_TRUE;
    }
    assert(res == ENUMERATION_INTERRUPTED);
    return TOY_FALSE;
}
