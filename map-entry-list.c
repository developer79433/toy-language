#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "mymalloc.h"
#include "buf-list.h"
#include "map-entry-list.h"
#include "str.h"
#include "expr.h"
#include "log.h"

toy_map_entry *map_entry_list_payload(toy_map_entry_list *list)
{
    return buf_list_payload_typed((toy_buf_list *) list, toy_map_entry);
}

const toy_map_entry *map_entry_list_payload_const(const toy_map_entry_list *list)
{
    return buf_list_payload_const_typed((const toy_buf_list *) list, toy_map_entry);
}

enumeration_result map_entry_list_foreach(toy_map_entry_list *list, toy_map_entry_list_item_callback callback, void *cookie)
{
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);
}

enumeration_result map_entry_list_foreach_const(const toy_map_entry_list *list, const_toy_map_entry_list_item_callback callback, void *cookie)
{
    return buf_list_foreach_const((const toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);
}

toy_map_entry_list *map_entry_list_alloc_ref(toy_str first_key, toy_expr *first_value)
{
    toy_map_entry entry = { .key = first_key, .value = first_value };
    assert(offsetof(toy_map_entry_list, next) == offsetof(toy_buf_list, next));
    assert(offsetof(toy_map_entry_list, entry) == offsetof(toy_buf_list, c));
    return (toy_map_entry_list *) buf_list_alloc(&entry, sizeof(entry));
}

toy_map_entry_list *map_entry_list_concat(toy_map_entry_list *orig, toy_map_entry_list *new_list)
{
    assert(offsetof(toy_map_entry_list, next) == offsetof(generic_list, next));
    return (toy_map_entry_list *) buf_list_concat((toy_buf_list *) orig, (toy_buf_list *) new_list);
}

typedef struct map_entry_dump_cb_args_struct {
    toy_bool printed_anything;
} map_entry_dump_cb_args;

static item_callback_result map_entry_dump_callback(void *cookie, size_t index, const toy_map_entry_list *item)
{
    map_entry_dump_cb_args *args = (map_entry_dump_cb_args *) cookie;
    const toy_map_entry *entry = map_entry_list_payload_const(item);
    if (args->printed_anything) {
        log_puts(", ");
    } else {
        log_putc(' ');
    }
    str_dump(entry->key);
    log_puts(": ");
    expr_dump(entry->value);
    args->printed_anything = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void map_entry_list_dump(const toy_map_entry_list *list)
{
    log_putc('{');
    map_entry_dump_cb_args args = { .printed_anything = TOY_FALSE };
    enumeration_result res = map_entry_list_foreach_const(list, map_entry_dump_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
    if (args.printed_anything) {
        log_putc(' ');
    }
    log_putc('}');
}
