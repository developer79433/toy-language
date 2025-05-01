#include <assert.h>
#include <string.h>
#include <stddef.h>

#include "str.h"
#include "buf-list.h"
#include "str-list-inline.h"

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
