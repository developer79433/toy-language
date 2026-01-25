#include <assert.h>

#include "test-str-list.h"
#include "str.h"
#include "str-list.h"
#include "list-visitor.h"

static item_callback_result str_list_item_callback(list_visitor *list_vis, size_t index, const toy_str_list *list)
{
    // log_printf("Got string '%s'\n", list->str);
    assert(index == 0 || index == 1);
    assert(index != 0 || str_equal(list->str, "first string"));
    assert(index != 1 || str_equal(list->str, "second string"));
    return CONTINUE_ENUMERATION;
}

void test_str_lists(void)
{
    toy_str first_str = "first string";
    toy_str_list *str_list = str_list_alloc(first_str);
    assert(str_equal(str_list->str, first_str));
    assert(NULL == str_list->next);
    assert(1 == str_list_len(str_list));
    toy_str second_str = "second string";
    toy_str_list *retval = str_list_append(str_list, second_str);
    assert(str_equal(retval->str, first_str));
    assert(NULL != retval->next);
    assert(str_equal(retval->next->str, second_str));
    assert(NULL == retval->next->next);
    assert(2 == str_list_len(retval));
    const_list_visitor list_vis = { .visit_entry = (const_list_entry_visit_func) str_list_item_callback };
    enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &list_vis, (const generic_list *) str_list);
    assert(res == ENUMERATION_COMPLETE);
    str_list_free(retval);
}
