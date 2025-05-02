#include <assert.h>

#include "str.h"
#include "test-str-list-inline.h"
#include "str-list-inline.h"

static item_callback_result str_list_inline_item_callback(void *cookie, size_t index, const toy_str_list_inline *list)
{
    size_t *count = (size_t *) cookie;
    // log_printf("Got string '%s'\n", list->str);
    assert(*count == 0 || *count == 1);
    assert((*count != 0) || (toy_str_equal(str_list_inline_payload_const(list), "first string")));
    assert((*count != 1) || (toy_str_equal(str_list_inline_payload_const(list), "second string")));
    (*count)++;
    return CONTINUE_ENUMERATION;
}

void test_str_list_inlines(void)
{
    toy_str first_str = "first string";
    toy_str_list_inline *str_list = str_list_inline_alloc(first_str);
    assert(toy_str_equal(&str_list->c, first_str));
    assert(NULL == str_list->next);
    assert(1 == str_list_inline_len(str_list));
    toy_str second_str = "second string";
    toy_str_list_inline *retval = str_list_inline_append(str_list, second_str);
    assert(toy_str_equal(&retval->c, first_str));
    assert(NULL != retval->next);
    assert(toy_str_equal(&retval->next->c, second_str));
    assert(NULL == retval->next->next);
    assert(2 == str_list_inline_len(retval));
    size_t count = 0;
    str_list_inline_foreach_const(str_list, str_list_inline_item_callback, &count);
    str_list_inline_free(retval);
}
