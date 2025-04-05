#include <stdio.h>

#include "mymalloc.h"
#include "toy-expr-tuple-list.h"
#include "dump.h"

toy_expr_tuple_list *alloc_expr_tuple_list(toy_expr *first_key, toy_expr *first_value)
{
    toy_expr_tuple_list *tuple_list;
    tuple_list = mymalloc(toy_expr_tuple_list);
    tuple_list->key = first_key;
    tuple_list->value = first_value;
    tuple_list->next = NULL;
    return tuple_list;
}

toy_expr_tuple_list *append_expr_tuple(toy_expr_tuple_list *orig, toy_expr_tuple_list *new_entry)
{
    toy_expr_tuple_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_entry;
    return orig;
}

void dump_expr_tuple_list(FILE *f, const toy_expr_tuple_list *list)
{
    int printed_anything = 0;
    fputc('{', f);
    if (list) {
        for (const toy_expr_tuple_list *cur = list; cur; cur = cur->next) {
            if (printed_anything) {
                fputs(", ", f);
            } else {
                fputc(' ', f);
            }
            dump_expr(f, cur->key);
            fputs(": ", f);
            dump_expr(f, cur->value);
            printed_anything = 1;
        }
        if (printed_anything) {
            fputc(' ', f);
        }
    }
    fputc('}', f);
}
