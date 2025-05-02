#include <assert.h>
#include <string.h>
#include <stddef.h>

#include "str.h"
#include "buf-list.h"
#include "str-list-inline.h"

toy_str str_list_inline_payload(toy_str_list_inline *list)
{
    return &list->c;
}

const toy_str str_list_inline_payload_const(const toy_str_list_inline *list)
{
    return (const toy_str) &list->c;
}

void str_list_inline_payload_set(const toy_str_list_inline *list, toy_str str)
{
    buf_list_payload_set((toy_buf_list *) list, str, strlen(str) + 1);
}

toy_str_list_inline *str_list_inline_alloc(const char *str)
{
    assert(offsetof(toy_str_list_inline, c) == offsetof(toy_buf_list, c));
    return (toy_str_list_inline *) buf_list_alloc((void *) str, strlen(str) + 1);
}

toy_str_list_inline *str_list_inline_append(toy_str_list_inline *list, toy_str new_item)
{
    assert(offsetof(toy_str_list_inline, next) == offsetof(toy_buf_list, next));
    assert(offsetof(toy_str_list_inline, c) == offsetof(toy_buf_list, c));
    return (toy_str_list_inline *) buf_list_append((toy_buf_list *) list, new_item, strlen(new_item) + 1);
}

size_t str_list_inline_len(const toy_str_list_inline *list)
{
    return buf_list_len((toy_buf_list *) list);
}

void str_list_inline_free(toy_str_list_inline *list)
{
    buf_list_free((toy_buf_list *) list);
}

list_iter_result str_list_inline_foreach(toy_str_list_inline *list, toy_str_list_inline_item_callback callback, void *cookie)
{
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);    
}

list_iter_result str_list_inline_foreach_const(const toy_str_list_inline *list, const_toy_str_list_inline_item_callback callback, void *cookie)
{
    return buf_list_foreach_const((toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);    
}
