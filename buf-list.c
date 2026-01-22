#include <string.h>

#include "generic-list.h"
#include "buf-list.h"
#include "mymalloc.h"

void *buf_list_payload(toy_buf_list *list)
{
    return &list->c;
}

const void *buf_list_payload_const(const toy_buf_list *list)
{
    return &list->c;
}

void buf_list_payload_set(toy_buf_list *list, const void *buf, size_t buf_size)
{
    memcpy(buf_list_payload(list), buf, buf_size);
}

void buf_list_init(toy_buf_list *list)
{
    list->c = 0;
}

void buf_list_payload_set_2(toy_buf_list *list, const void *buf1, size_t buf1_size, const void *buf2, size_t buf2_size)
{
    memcpy_2(buf_list_payload(list), buf1, buf1_size, buf2, buf2_size);
}

toy_buf_list *buf_list_alloc(const void *buf, size_t buf_size)
{
    toy_buf_list *new_list = (toy_buf_list *) generic_list_alloc_size(buf_size);
    buf_list_payload_set(new_list, buf, buf_size);
    return new_list;
}

toy_buf_list *buf_list_alloc_2(const void *buf1, size_t buf1_size, const void *buf2, size_t buf2_size)
{
    toy_buf_list *new_list = (toy_buf_list *) generic_list_alloc_size(buf1_size + buf2_size);
    buf_list_payload_set_2(new_list, buf1, buf1_size, buf2, buf2_size);
    return new_list;
}

toy_buf_list *buf_list_concat(toy_buf_list *list, toy_buf_list *new_list)
{
    return (toy_buf_list *) generic_list_concat((generic_list *) list, (generic_list *) new_list);
}

toy_buf_list *buf_list_append(toy_buf_list *list, const void *buf, size_t buf_size)
{
    toy_buf_list *new_list = buf_list_alloc(buf, buf_size);
    return (toy_buf_list *) generic_list_concat((generic_list *) list, (generic_list *) new_list);
}

toy_buf_list *buf_list_prepend(toy_buf_list *list, const void *buf, size_t buf_size)
{
    toy_buf_list *new_list = buf_list_alloc(buf, buf_size);
    return (toy_buf_list *) generic_list_concat((generic_list *) new_list, (generic_list *) list);
}

toy_buf_list *buf_list_remove_first(toy_buf_list *list, toy_buf_list **removed)
{
    return (toy_buf_list *) generic_list_remove_first((generic_list *) list, (generic_list **) removed);
}

toy_buf_list *buf_list_remove_last(toy_buf_list *list, toy_buf_list **removed)
{
    return (toy_buf_list *) generic_list_remove_last((generic_list *) list, (generic_list **) removed);
}

size_t buf_list_len(const toy_buf_list *list)
{
    return generic_list_len((generic_list *) list);
}

void *buf_list_index(toy_buf_list *list, size_t index)
{
    toy_buf_list *found = (toy_buf_list *) generic_list_index((generic_list *) list, index);
    if (found) {
        return buf_list_payload(found);
    }
    return NULL;
}

const void *buf_list_index_const(const toy_buf_list *list, size_t index)
{
    const toy_buf_list *found = (const toy_buf_list *) generic_list_index_const((const generic_list *) list, index);
    if (found) {
        return buf_list_payload_const(found);
    }
    return NULL;
}

enumeration_result buf_list_foreach(toy_buf_list *list, buf_list_item_callback callback, void *cookie)
{
    return generic_list_foreach((generic_list *) list, (generic_list_item_callback) callback, cookie);
}

enumeration_result buf_list_foreach_const(const toy_buf_list *list, const_buf_list_item_callback callback, void *cookie)
{
    return generic_list_foreach_const((generic_list *) list, (const_generic_list_item_callback) callback, cookie);
}

toy_bool buf_list_none_match(const toy_buf_list *list, buf_list_filter_func filter, void *cookie)
{
    return list_none_match((const generic_list *) list, (generic_list_filter_func) filter, cookie);
}

toy_bool buf_list_all_match(const toy_buf_list *list, buf_list_filter_func filter, void *cookie)
{
    return list_all_match((const generic_list *) list, (generic_list_filter_func) filter, cookie);
}

void buf_list_free(toy_buf_list *list)
{
    generic_list_free((generic_list *) list);
}
