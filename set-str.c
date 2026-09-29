#include <assert.h>
#include <string.h>

#include "set-str.h"
#include "set-buf.h"
#include "mymalloc.h"
#include "map-visitor.h"
#include "log.h"

/* TODO: Delete me */
static const int dummy_value = 1;

set_str *set_str_alloc(void)
{
    set_str *set = (set_str *) set_buf_alloc();
    set_str_init(set);
    return set;
}

void set_str_init(set_str *set)
{
    set_buf_init(&set->set);
}

typedef struct set_str_dump_vis_struct {
    map_visitor map_vis;
    toy_bool printed_anything;
} set_str_dump_vis;

static item_callback_result dump_entry(set_str_dump_vis *visitor, set_str_entry *entry)
{
    assert(entry->value == dummy_value);
    if (visitor->printed_anything) {
        log_debug(", ");
    }
    assert(entry->key_len == strlen(entry->key) + 1);
    str_dump(entry->key, TOY_FALSE);
    visitor->printed_anything = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void set_str_dump(set_str *set)
{
    set_str_dump_vis dump_vis = { .map_vis.visit_entry = (map_entry_visit_func) dump_entry, .printed_anything = TOY_FALSE };
    log_debug("{ ");
    enumeration_result res = map_visitor_visit_map(&dump_vis.map_vis, (generic_map *) &set->set);
    log_debug("} ");
    assert(ENUMERATION_COMPLETE == res);
}

toy_bool set_str_contains(const set_str *set, const toy_str str)
{
    return set_buf_contains(&set->set, str, strlen(str) + 1);
}

void set_str_add(set_str *set, const toy_str str)
{
    return set_buf_add(&set->set, str, strlen(str) + 1);
}

void set_str_remove(set_str *set, const toy_str str)
{
    return set_buf_remove(&set->set, str, strlen(str) + 1);
}

void set_str_free(set_str *set)
{
    return set_buf_free(&set->set);
}
