#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mymalloc.h"
#include "generic-list.h"
#include "toy-expr-list.h"
#include "dump.h"

toy_expr_list *alloc_expr_list(toy_expr *first_elem)
{
    toy_expr_list *list;
    list = mymalloc(toy_expr_list);
    list->expr = first_elem;
    list->next = NULL;
    return list;
}

size_t expr_list_len(const toy_expr_list *list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return generic_list_len((const generic_list *) list);
}

void dump_expr_list(FILE *f, toy_expr_list *list)
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

toy_expr_list *append_expr_list(toy_expr_list *orig, toy_expr_list *new_list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_expr_list *) generic_list_append((generic_list *) orig, (generic_list *) new_list);
}
