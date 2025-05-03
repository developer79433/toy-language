#include <assert.h>

#include "test-str-list.h"
#include "str.h"
#include "str-list.h"

static item_callback_result str_list_item_callback(void *cookie, size_t index, const toy_str_list *list)
{
    size_t *count = (size_t *) cookie;
    // log_printf("Got string '%s'\n", list->str);
    assert(*count == 0 || *count == 1);
    assert((*count != 0) || (toy_str_equal(list->str, "first string")));
    assert((*count != 1) || (toy_str_equal(list->str, "second string")));
    (*count)++;
    return CONTINUE_ENUMERATION;
}

void test_str_lists(void)
{
    toy_str first_str = "first string";
    toy_str_list *str_list = str_list_alloc(first_str);
    assert(toy_str_equal(str_list->str, first_str));
    assert(NULL == str_list->next);
    assert(1 == str_list_len(str_list));
    toy_str second_str = "second string";
    toy_str_list *retval = str_list_append(str_list, second_str);
    assert(toy_str_equal(retval->str, first_str));
    assert(NULL != retval->next);
    assert(toy_str_equal(retval->next->str, second_str));
    assert(NULL == retval->next->next);
    assert(2 == str_list_len(retval));
    size_t count = 0;
    enumeration_result res = str_list_foreach_const(str_list, str_list_item_callback, &count);
    assert(res == ENUMERATION_COMPLETE);
    str_list_free(retval);
}
