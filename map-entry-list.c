#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "mymalloc.h"
#include "buf-list.h"
#include "map-entry-list.h"
#include "dump.h"
#include "str.h"

toy_map_entry *map_entry_list_payload(toy_map_entry_list *list)
{
    return buf_list_payload_typed((toy_buf_list *) list, toy_map_entry);
}

const toy_map_entry *map_entry_list_payload_const(const toy_map_entry_list *list)
{
    return buf_list_payload_const_typed((const toy_buf_list *) list, toy_map_entry);
}

enumeration_result map_entry_list_foreach(toy_map_entry_list *list, toy_map_entry_list_item_callback callback, void *cookie)
{
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);
}

enumeration_result map_entry_list_foreach_const(const toy_map_entry_list *list, const_toy_map_entry_list_item_callback callback, void *cookie)
{
    return buf_list_foreach_const((const toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);
}

toy_map_entry_list *map_entry_list_alloc_ref(toy_str first_key, toy_expr *first_value)
{
    toy_map_entry entry = { .key = first_key, .value = first_value };
    assert(offsetof(toy_map_entry_list, next) == offsetof(toy_buf_list, next));
    assert(offsetof(toy_map_entry_list, entry) == offsetof(toy_buf_list, c));
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
            dump_str(f, cur->entry.key);
            fputs(": ", f);
            expr_dump(f, cur->entry.value);
            printed_anything = 1;
        }
        if (printed_anything) {
            fputc(' ', f);
        }
    }
    fputc('}', f);
}
