#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "mymalloc.h"
#include "generic-list.h"
#include "toy-map-entry-list.h"
#include "dump.h"
#include "toy-str.h"

toy_map_entry_list *map_entry_list_alloc_ref(toy_str first_key, toy_expr *first_value)
{
    toy_map_entry_list *tuple_list;
    tuple_list = mymalloc(toy_map_entry_list);
    tuple_list->key = first_key;
    tuple_list->value = first_value;
    tuple_list->next = NULL;
    return tuple_list;
}

toy_map_entry_list *map_entry_list_concat(toy_map_entry_list *orig, toy_map_entry_list *new_list)
{
    assert(offsetof(toy_map_entry_list, next) == offsetof(generic_list, next));
    return (toy_map_entry_list *) generic_list_concat((generic_list *) orig, (generic_list *) new_list);
}

void map_entry_list_dump(FILE *f, const toy_map_entry_list *list)
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
