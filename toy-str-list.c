#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "generic-list.h"
#include "toy-str.h"
#include "toy-str-list.h"

void str_list_dump(FILE *f, const toy_str_list *list)
{
    const toy_str_list *cur;
    if (list) {
        int output_something = 0;
        for (cur = list; cur; cur = cur->next) {
            if (output_something) {
                fputs(", ", f);
            }
            dump_str(f, cur->str);
            output_something = 1;
        }
    }
}

toy_str_list *str_list_alloc_ref(const char *str)
{
    assert(offsetof(toy_str_list, str) == offsetof(generic_list, payload));
    return (toy_str_list *) generic_list_alloc_ref((void *) str);
}

toy_str_list *str_list_alloc_own(const char *str)
{
    assert(offsetof(toy_str_list, str) == offsetof(generic_list, payload));
    return (toy_str_list *) generic_list_alloc_own((void *) str, strlen(str) + 1);
}

toy_str_list *str_list_concat(toy_str_list *orig, toy_str_list *new_list)
{
    assert(offsetof(toy_str_list, next) == offsetof(generic_list, next));
    return (toy_str_list *) generic_list_concat((generic_list *) orig, (generic_list *) new_list);
}

toy_str_list *str_list_append_ref(toy_str_list *list, toy_str new_item)
{
    assert(offsetof(toy_str_list, next) == offsetof(generic_list, next));
    assert(offsetof(toy_str_list, str) == offsetof(generic_list, payload));
    return (toy_str_list *) generic_list_append_ref((generic_list *) list, new_item);
}

toy_str_list *str_list_append_own(toy_str_list *list, toy_str new_item)
{
    assert(offsetof(toy_str_list, next) == offsetof(generic_list, next));
    assert(offsetof(toy_str_list, str) == offsetof(generic_list, payload));
    return (toy_str_list *) generic_list_append_own((generic_list *) list, new_item, strlen(new_item) + 1);
}

size_t str_list_len(const toy_str_list *list)
{
    assert(offsetof(toy_str_list, next) == offsetof(generic_list, next));
    return generic_list_len((const generic_list *) list);
}
