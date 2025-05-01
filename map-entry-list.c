#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "mymalloc.h"
#include "buf-list.h"
#include "map-entry-list.h"
#include "dump.h"
#include "str.h"

typedef struct toy_map_entry_struct {
    toy_str key;
    toy_expr *value;
} toy_map_entry;

toy_map_entry_list *map_entry_list_alloc_ref(toy_str first_key, toy_expr *first_value)
{
    toy_map_entry entry = { .key = first_key, .value = first_value };
    assert(offsetof(toy_map_entry_list, next) == offsetof(toy_buf_list, next));
    assert(offsetof(toy_map_entry, key) == offsetof(toy_buf_list, c));
    return (toy_map_entry_list *) buf_list_alloc(&entry, sizeof(entry));
}

toy_map_entry_list *map_entry_list_concat(toy_map_entry_list *orig, toy_map_entry_list *new_list)
{
    assert(offsetof(toy_map_entry_list, next) == offsetof(generic_list, next));
    return (toy_map_entry_list *) buf_list_concat((toy_buf_list *) orig, (toy_buf_list *) new_list);
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
