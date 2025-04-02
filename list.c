#include <stddef.h>

#include "list.h"

size_t list_len(const toy_list *list)
{
    size_t size;
    for (size = 1; list->next; list = list->next) { size++; }
    return size;
}

toy_expr *list_index(toy_list *list, toy_num index)
{
    /* FIXME: inefficient */
    for (size_t i = 0; list; list = list->next, i++) {
        if (i == index) {
            return list->expr;
        }
    }
    invalid_list_index(list, index);
    return NULL;
}

size_t str_list_len(const toy_str_list *list)
{
    size_t size;
    for (size = 1; list->next; list = list->next) { size++; }
    return size;
}
