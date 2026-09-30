#include <assert.h>
#include <string.h>

#include "set-buf.h"
#include "map-buf-buf.h"
#include "mymalloc.h"
#include "map-visitor.h"
#include "log.h"

set_buf *set_buf_alloc(void)
{
    set_buf *set = mymalloc(set_buf);
    set_buf_init(set);
    return set;
}

void set_buf_init(set_buf *set)
{
    map_buf_buf_init(&set->map);
}

typedef struct set_buf_dump_vis_struct {
    map_visitor map_vis;
    toy_bool printed_anything;
} set_buf_dump_vis;

static item_callback_result dump_entry(set_buf_dump_vis *visitor, set_buf_entry *entry)
{
    if (visitor->printed_anything) {
        log_debug(", ");
    }
    hex_dump(entry->key, entry->key_len);
    visitor->printed_anything = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void set_buf_dump(set_buf *set)
{
    set_buf_dump_vis dump_vis = { .map_vis.visit_entry = (map_entry_visit_func) dump_entry, .printed_anything = TOY_FALSE };
    log_debug("{ ");
    enumeration_result res = map_visitor_visit_map(&dump_vis.map_vis, (generic_map *) &set->map);
    log_debug("} ");
    assert(ENUMERATION_COMPLETE == res);
}

toy_bool set_buf_contains(const set_buf *set, const void *buf, size_t size)
{
    const void *val = map_buf_buf_get_const(&set->map, buf, size);
    if (val) {
        return TOY_TRUE;
    }
    return TOY_FALSE;
}

void set_buf_add(set_buf *set, const void *buf, size_t size)
{
    assert(!set_buf_contains(set, buf, size));
    set_result set_res = map_buf_buf_set(&set->map, buf, size, NULL, 0);
    assert(SET_NEW == set_res);
}

void set_buf_remove(set_buf *set, const void *buf, size_t size)
{
    delete_result del_res = map_buf_buf_delete(&set->map, buf, size);
    assert(DELETED == del_res);
}

void set_buf_free(set_buf *set)
{
    return map_buf_buf_free(&set->map);
}
