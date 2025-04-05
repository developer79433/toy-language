#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "generic-list.h"
#include "toy-str.h"
#include "toy-str-list.h"

void dump_str_list(FILE *f, const toy_str_list *list)
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

toy_str_list *alloc_str_list(const char *str)
{
    size_t size = sizeof(toy_str_list) + strlen(str) + 1;
    void *mem = malloc(size);
    toy_str_list *list = (toy_str_list *) mem;
    list->next = NULL;
    list->str = (char *) (list + 1);
    strcpy(list->str, str);
    return list;
}

toy_str_list *append_str_list(toy_str_list *orig, toy_str_list *new_list)
{
    assert(offsetof(toy_str_list, next) == offsetof(generic_list, next));
    return (toy_str_list *) generic_list_append((generic_list *) orig, (generic_list *) new_list);
}

size_t str_list_len(const toy_str_list *list)
{
    assert(offsetof(toy_str_list, next) == offsetof(generic_list, next));
    return generic_list_len((const generic_list *) list);
}
