#include <stddef.h>

#include "list.h"

size_t list_len(const toy_list *list)
{
    size_t size;
    for (size = 1; list->next; list = list->next) { size++; }
    return size;
}
