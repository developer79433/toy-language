#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mymalloc.h"
#include "ptr-list.h"
#include "expr-list.h"
#include "dump.h"

toy_expr *expr_list_payload(toy_expr_list *list)
{
    return list->expr;
}

const void *expr_list_payload_const(const toy_expr_list *list)
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

void expr_list_dump(FILE *f, const toy_expr_list *list)
{
    int printed_anything = 0;
    fputc('[', f);
    if (list) {
        /* TODO: Use expr_list_foreach_const */
        for (const toy_expr_list *cur = list; cur; cur = cur->next) {
            if (printed_anything) {
                fputs(", ", f);
            } else {
                fputc(' ', f);
            }
            expr_dump(f, cur->expr);
            printed_anything = 1;
        }
        if (printed_anything) {
            fputc(' ', f);
        }
    }
    fputc(']', f);
}

toy_expr_list *expr_list_concat(toy_expr_list *orig, toy_expr_list *new_list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_expr_list *) ptr_list_concat((toy_ptr_list *) orig, (toy_ptr_list *) new_list);
}

enumeration_result expr_list_foreach(toy_expr_list *list, toy_expr_list_item_callback callback, void *cookie)
{
    return ptr_list_foreach((toy_ptr_list *) list, (ptr_list_item_callback) callback, cookie);
}

enumeration_result expr_list_foreach_const(const toy_expr_list *list, const_toy_expr_list_item_callback callback, void *cookie)
{
    return ptr_list_foreach_const((const toy_ptr_list *) list, (const_ptr_list_item_callback) callback, cookie);
}
