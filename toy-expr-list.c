#include <stdlib.h>

#include "mymalloc.h"
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
    /* TODO: return list_generic_len(args); */
    size_t size;
    for (size = 0; list; list = list->next) {
        size++;
    }
    return size;
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

/* TODO: Handle orig being null, so the caller in parser.y doesn't have to repeatedly do so */
toy_expr_list *append_expr_list(toy_expr_list *orig, toy_expr_list *new_item)
{
    toy_expr_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_item;
    return orig;
}
