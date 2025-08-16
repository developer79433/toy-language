#include <assert.h>

#include "map-size-t.h"
#include "map-buf.h"
#include "log.h"

void map_size_t_init(map_size_t *map) {
    map_buf_init((map_buf *) map);
}

map_size_t *map_size_t_alloc(void) {
    return (map_size_t *) map_buf_alloc();
}

void map_size_t_assert_valid(const map_size_t *map) {
    map_buf_assert_valid((map_buf *) map);
}

size_t *map_size_t_get(map_size_t *map, const toy_str key) {
    return (size_t *) map_buf_get((map_buf *) map, key);
}

const size_t *map_size_t_get_const(const map_size_t *map, const toy_str key) {
    return (const size_t *) map_buf_get_const((const map_buf *) map, key);
}

set_result map_size_t_set(map_size_t *map, const toy_str key, size_t value) {
    return map_buf_set((map_buf *) map, key, &value, sizeof(value));
}

delete_result map_size_t_delete(map_size_t *map, const toy_str key) {
    return map_buf_delete((map_buf *) map, key);
}

enumeration_result map_size_t_foreach_const(const map_size_t *map, const_map_size_t_entry_callback callback, void *cookie) {
    return map_buf_foreach_const((const map_buf *) map, (const_map_buf_entry_callback) callback, cookie);
}

enumeration_result map_size_t_foreach(map_size_t *map, map_size_t_entry_callback callback, void *cookie) {
    return map_buf_foreach((map_buf *) map, (map_buf_entry_callback) callback, cookie);
}

size_t map_size_t_size(const map_size_t *map) {
    return map_buf_size((const map_buf *) map);
}

static item_callback_result map_size_t_entry_dump_cb(void *cookie, const map_size_t_entry *item)
{
    log_printf("%s => %zu", item->key, item->s);
    return CONTINUE_ENUMERATION;
}

void map_size_t_dump(const map_size_t *map) {
    enumeration_result res = map_size_t_foreach_const(map, map_size_t_entry_dump_cb, NULL);
    assert(res == ENUMERATION_COMPLETE);
}

void map_size_t_reset(map_size_t *map) {
    return map_buf_reset((map_buf *) map);
}

void map_size_t_free(map_size_t *map) {
    return map_buf_free((map_buf *) map);
}
