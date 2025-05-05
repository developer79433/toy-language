#include <string.h>

#include "generic-list.h"
#include "buf-list.h"

void *buf_list_payload(toy_buf_list *list)
{
    return &list->c;
}

const void *buf_list_payload_const(const toy_buf_list *list)
{
    return &list->c;
}

void buf_list_payload_set(toy_buf_list *list, void *buf, size_t buf_size)
{
    memcpy(buf_list_payload(list), buf, buf_size);
}

void buf_list_payload_set_2(toy_buf_list *list, void *buf1, size_t buf1_size, void *buf2, size_t buf2_size)
{
    char *p = (char *) buf_list_payload(list);
    if (buf1) {
        memcpy(p, buf1, buf1_size);
        if (buf2) {
            p += buf1_size;
            memcpy(p, buf2, buf2_size);
        }
    }
}

toy_buf_list *buf_list_alloc(void *buf, size_t buf_size)
{
    toy_buf_list *new_list = (toy_buf_list *) generic_list_alloc_size(buf_size);
    if (buf) {
        buf_list_payload_set(new_list, buf, buf_size);
    }
    return new_list;
}

toy_buf_list *buf_list_alloc_2(void *buf1, size_t buf1_size, void *buf2, size_t buf2_size)
{
    toy_buf_list *new_list = (toy_buf_list *) generic_list_alloc_size(buf1_size + buf2_size);
    buf_list_payload_set_2(new_list, buf1, buf1_size, buf2, buf2_size);
    return new_list;
}

toy_buf_list *buf_list_concat(toy_buf_list *list, toy_buf_list *new_list)
{
    return (toy_buf_list *) generic_list_concat((generic_list *) list, (generic_list *) new_list);
}

toy_buf_list *buf_list_append(toy_buf_list *list, void *buf, size_t buf_size)
{
    toy_buf_list *new_list = buf_list_alloc(buf, buf_size);
    return (toy_buf_list *) generic_list_concat((generic_list *) list, (generic_list *) new_list);
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

enumeration_result buf_list_foreach(toy_buf_list *list, buf_list_item_callback callback, void *cookie)
{
    return generic_list_foreach((generic_list *) list, (generic_list_item_callback) callback, cookie);
}

enumeration_result buf_list_foreach_const(const toy_buf_list *list, const_buf_list_item_callback callback, void *cookie)
{
    return generic_list_foreach_const((generic_list *) list, (const_generic_list_item_callback) callback, cookie);
}

void buf_list_free(toy_buf_list *list)
{
    generic_list_free((generic_list *) list);
}
