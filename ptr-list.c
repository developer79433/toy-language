#include <string.h>

#include "mymalloc.h"
#include "generic-list.h"
#include "list-visitor.h"
#include "ptr-list.h"

void *ptr_list_payload(toy_ptr_list *list)
{
    return list->ptr;
}

const void *ptr_list_payload_const(const toy_ptr_list *list)
{
    return list->ptr;
}

void ptr_list_set_payload(toy_ptr_list *list, void *new_payload)
{
    list->ptr = new_payload;
}

void ptr_list_init(toy_ptr_list *list)
{
    list->ptr = NULL;
}

toy_ptr_list *ptr_list_alloc(void *ptr)
{
    toy_ptr_list *list = (toy_ptr_list *) list_alloc_size(sizeof(list->ptr));
    ptr_list_set_payload(list, ptr);
    return list;
}

void *ptr_list_index(toy_ptr_list *list, size_t index)
{
    toy_ptr_list *found = (toy_ptr_list *) list_index((generic_list *) list, index);
    if (found) {
        return ptr_list_payload(found);
    }
    return NULL;
}

toy_ptr_list *ptr_list_append(toy_ptr_list *list, void *new_ptr)
{
    toy_ptr_list *new_list = ptr_list_alloc(new_ptr);
    return (toy_ptr_list *) list_concat((generic_list *) list, (generic_list *) new_list);
}

toy_ptr_list *ptr_list_concat(toy_ptr_list *list, toy_ptr_list *new_list)
{
    return (toy_ptr_list *) list_concat((generic_list *) list, (generic_list *) new_list);
}

toy_ptr_list *ptr_list_prepend(toy_ptr_list *list, void *new_ptr)
{
    toy_ptr_list *new_list = ptr_list_alloc(new_ptr);
    return (toy_ptr_list *) list_concat((generic_list *) new_list, (generic_list *) list);
}

toy_ptr_list *ptr_list_remove_first(toy_ptr_list *list, toy_ptr_list **removed)
{
    return (toy_ptr_list *) list_remove_first((generic_list *) list, (generic_list **) removed);
}

toy_ptr_list *ptr_list_remove_last(toy_ptr_list *list, toy_ptr_list **removed)
{
    return (toy_ptr_list *) list_remove_last((generic_list *) list, (generic_list **) removed);
}

size_t ptr_list_len(const toy_ptr_list *list)
{
    return list_len((generic_list *) list);
}

void ptr_list_free(toy_ptr_list *list)
{
    list_free((generic_list *) list);
}
