#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "mymalloc.h"
#include "buf-list.h"
#include "map-expr-entry-list.h"
#include "str.h"
#include "expr.h"
#include "log.h"
#include "list-visitor.h"

toy_map_expr_entry *map_expr_entry_list_payload(toy_map_expr_entry_list *list)
{
    return buf_list_payload_typed((toy_buf_list *) list, toy_map_expr_entry);
}

const toy_map_expr_entry *map_expr_entry_list_payload_const(const toy_map_expr_entry_list *list)
{
    return buf_list_payload_const_typed((const toy_buf_list *) list, toy_map_expr_entry);
}

toy_map_expr_entry_list *map_expr_entry_list_alloc_ref(toy_str first_key, toy_expr *first_value)
{
    toy_map_expr_entry entry = { .key = first_key, .expr = first_value };
    assert(offsetof(toy_map_expr_entry_list, next) == offsetof(toy_buf_list, next));
    assert(offsetof(toy_map_expr_entry_list, entry) == offsetof(toy_buf_list, c));
    return (toy_map_expr_entry_list *) buf_list_alloc(&entry, sizeof(entry));
}

toy_map_expr_entry_list *map_expr_entry_list_concat(toy_map_expr_entry_list *orig, toy_map_expr_entry_list *new_list)
{
    assert(offsetof(toy_map_expr_entry_list, next) == offsetof(generic_list, next));
    return (toy_map_expr_entry_list *) buf_list_concat((toy_buf_list *) orig, (toy_buf_list *) new_list);
}

typedef struct map_entry_dump_visitor_struct {
    const_list_visitor list_vis;
    toy_bool printed_anything;
} map_entry_dump_visitor;

static item_callback_result map_entry_dump_callback(map_entry_dump_visitor *dump_vis, size_t index, const toy_map_expr_entry_list *item)
{
    const toy_map_expr_entry *entry = map_expr_entry_list_payload_const(item);
    if (dump_vis->printed_anything) {
        log_puts(", ");
    } else {
        log_putc(' ');
    }
    str_dump(entry->key, TOY_FALSE);
    log_puts(": ");
    expr_dump(entry->expr);
    dump_vis->printed_anything = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void map_expr_entry_list_dump(const toy_map_expr_entry_list *list)
{
    log_putc('{');
    map_entry_dump_visitor dump_vis = { .list_vis.visit_entry = (const_list_entry_visit_func) map_entry_dump_callback, .printed_anything = TOY_FALSE };
    enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &dump_vis, (const generic_list *) list);
    assert(ENUMERATION_COMPLETE == res);
    if (dump_vis.printed_anything) {
        log_putc(' ');
    }
    log_putc('}');
}
