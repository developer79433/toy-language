#include <stdio.h>

#include "mymalloc.h"
#include "toy-map-entry-list.h"
#include "dump.h"
#include "toy-str.h"

toy_map_entry_list *alloc_map_entry_list(toy_str first_key, toy_expr *first_value)
{
    toy_map_entry_list *tuple_list;
    tuple_list = mymalloc(toy_map_entry_list);
    tuple_list->key = first_key;
    tuple_list->value = first_value;
    tuple_list->next = NULL;
    return tuple_list;
}

toy_map_entry_list *append_map_entry(toy_map_entry_list *orig, toy_map_entry_list *new_entry)
{
    toy_map_entry_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_entry;
    return orig;
}

void dump_map_entry_list(FILE *f, const toy_map_entry_list *list)
{
    int printed_anything = 0;
    fputc('{', f);
    if (list) {
        for (const toy_map_entry_list *cur = list; cur; cur = cur->next) {
            if (printed_anything) {
                fputs(", ", f);
            } else {
                fputc(' ', f);
            }
            dump_str(f, cur->key);
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
