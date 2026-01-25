#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mymalloc.h"
#include "ptr-list.h"
#include "expr-list.h"
#include "expr.h"
#include "log.h"
#include "list-visitor.h"

toy_expr *expr_list_payload(toy_expr_list *list)
{
    return list->expr;
}

const toy_expr *expr_list_payload_const(const toy_expr_list *list)
{
    return list->expr;
}

void expr_list_set_payload(toy_expr_list *list, toy_expr *expr)
{
    list->expr = expr;
}

toy_expr_list *expr_list_alloc(toy_expr *first_elem)
{
    assert(offsetof(toy_expr_list, expr) == offsetof(toy_ptr_list, ptr));
    return (toy_expr_list *) ptr_list_alloc(first_elem);
}

size_t expr_list_len(const toy_expr_list *list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(toy_ptr_list, next));
    return ptr_list_len((const toy_ptr_list *) list);
}

toy_expr_list *expr_list_append(toy_expr_list *list, toy_expr *new_expr)
{
    assert(offsetof(toy_expr_list, expr) == offsetof(toy_ptr_list, ptr));
    assert(offsetof(toy_expr_list, next) == offsetof(toy_ptr_list, next));
    return (toy_expr_list *) ptr_list_append((toy_ptr_list *) list, new_expr);
}

typedef struct expr_dump_visitor_struct {
    const_list_visitor list_vis;
    toy_bool printed_anything;
} expr_dump_visitor;

static item_callback_result expr_dump_callback(expr_dump_visitor *expr_dump_vis, size_t index, const toy_expr_list *item)
{
    const toy_expr *expr = expr_list_payload_const(item);
    if (expr_dump_vis->printed_anything) {
        log_puts(", ");
    } else {
        log_putc(' ');
    }
    expr_dump(expr);
    expr_dump_vis->printed_anything = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void expr_list_dump(const toy_expr_list *list, toy_bool include_braces)
{
    if (include_braces) {
        log_putc('[');
    }
    expr_dump_visitor expr_dump_vis = { .list_vis.visit_entry = (const_list_entry_visit_func) expr_dump_callback, .printed_anything = TOY_FALSE };
    enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &expr_dump_vis, (const generic_list *) list);
    assert(ENUMERATION_COMPLETE == res);
    for (const toy_expr_list *cur = list; cur; cur = cur->next) {
    }
    if (expr_dump_vis.printed_anything) {
        log_putc(' ');
    }
    if (include_braces) {
        log_putc(']');
    }
}

toy_expr_list *expr_list_concat(toy_expr_list *orig, toy_expr_list *new_list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_expr_list *) ptr_list_concat((toy_ptr_list *) orig, (toy_ptr_list *) new_list);
}
