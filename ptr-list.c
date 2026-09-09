#include <assert.h>
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

typedef struct ptr_list_reverse_visitor_struct {
    const_list_visitor list_vis;
    toy_ptr_list *reversed;
    size_t item_size;
} ptr_list_reverse_visitor;

static item_callback_result ptr_list_reverse_visitor_visit_entry(ptr_list_reverse_visitor *rev_vis, size_t index, const toy_ptr_list *item)
{
    void *ptr = (void *) ptr_list_payload_const(item);
    if (rev_vis->reversed) {
        rev_vis->reversed = ptr_list_append(rev_vis->reversed, ptr);
    } else {
        rev_vis->reversed = ptr_list_alloc(ptr);
    }
    return CONTINUE_ENUMERATION;
}

toy_ptr_list *ptr_list_reverse(const toy_ptr_list *list)
{
    ptr_list_reverse_visitor rev_vis = { .list_vis.visit_entry = (const_list_entry_visit_func) ptr_list_reverse_visitor_visit_entry, .list_vis.prev_item = NULL, .reversed = NULL };
    enumeration_result res = const_list_visitor_visit_list(&rev_vis.list_vis, (const generic_list *) list);
    assert(ENUMERATION_COMPLETE == res);
    return rev_vis.reversed;
}
