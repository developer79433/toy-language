#include <assert.h>

#include "str.h"
#include "test-str-list-inline.h"
#include "str-list-inline.h"
#include "list-visitor.h"

static item_callback_result str_list_inline_item_callback(const_list_visitor *list_Vis, size_t index, const toy_str_list_inline *list)
{
    // log_printf("Got string '%s'\n", list->str);
    assert(index == 0 || index == 1);
    assert(index != 0 || str_equal(str_list_inline_payload_const(list), "first string"));
    assert(index != 1 || str_equal(str_list_inline_payload_const(list), "second string"));
    return CONTINUE_ENUMERATION;
}

void test_str_list_inlines(void)
{
    toy_str first_str = "first string";
    toy_str_list_inline *str_list = str_list_inline_alloc(first_str);
    assert(str_equal(&str_list->c, first_str));
    assert(NULL == str_list->next);
    assert(1 == str_list_inline_len(str_list));
    toy_str second_str = "second string";
    toy_str_list_inline *retval = str_list_inline_append(str_list, second_str);
    assert(str_equal(&retval->c, first_str));
    assert(NULL != retval->next);
    assert(str_equal(&retval->next->c, second_str));
    assert(NULL == retval->next->next);
    assert(2 == str_list_inline_len(retval));
    const_list_visitor list_vis = { .visit_entry = (const_list_entry_visit_func) str_list_inline_item_callback };
    enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &list_vis, (const generic_list *) str_list);
    assert(res == ENUMERATION_COMPLETE);
    str_list_inline_free(retval);
}
