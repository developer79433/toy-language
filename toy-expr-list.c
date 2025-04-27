#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mymalloc.h"
#include "generic-list.h"
#include "toy-expr-list.h"
#include "dump.h"

toy_expr_list *expr_list_alloc_ref(toy_expr *first_elem)
{
    assert(offsetof(toy_expr_list, expr) == offsetof(generic_list, payload));
    return (toy_expr_list *) generic_list_alloc_ref(first_elem);
}

toy_expr_list *expr_list_alloc_own(toy_expr *first_elem)
{
    assert(offsetof(toy_expr_list, expr) == offsetof(generic_list, payload));
    return (toy_expr_list *) generic_list_alloc_own(first_elem, sizeof(*first_elem));
}

size_t expr_list_len(const toy_expr_list *list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return generic_list_len((const generic_list *) list);
}

toy_expr_list *expr_list_append_ref(toy_expr_list *list, toy_expr *new_expr)
{
    assert(offsetof(toy_expr_list, expr) == offsetof(generic_list, payload));
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_expr_list *) generic_list_append_ref((generic_list *) list, new_expr);
}

toy_expr_list *expr_list_append_own(toy_expr_list *list, toy_expr *new_expr)
{
    assert(offsetof(toy_expr_list, expr) == offsetof(generic_list, payload));
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_expr_list *) generic_list_append_own((generic_list *) list, new_expr, sizeof(*new_expr));
}

void expr_list_dump(FILE *f, toy_expr_list *list)
{
    int printed_anything = 0;
    fputc('[', f);
    if (list) {
        for (toy_expr_list *cur = list; cur; cur = cur->next) {
            if (printed_anything) {
                fputs(", ", f);
            } else {
                fputc(' ', f);
            }
            dump_expr(f, cur->expr);
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
    return (toy_expr_list *) generic_list_concat((generic_list *) orig, (generic_list *) new_list);
}
