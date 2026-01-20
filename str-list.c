#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ptr-list.h"
#include "str.h"
#include "str-list.h"
#include "log.h"

toy_str str_list_payload(toy_str_list *list)
{
    return list->str;
}

const toy_str str_list_payload_const(const toy_str_list *list)
{
    return list->str;
}

void str_list_dump(const toy_str_list *list)
{
    const toy_str_list *cur;
    if (list) {
        int output_something = 0;
        for (cur = list; cur; cur = cur->next) {
            if (output_something) {
                log_puts(", ");
            }
            str_dump(cur->str);
            output_something = 1;
        }
    }
}

toy_str str_list_index(toy_str_list *list, size_t index)
{
    assert(offsetof(toy_str_list, str) == offsetof(toy_ptr_list, ptr));
    return ptr_list_index((toy_ptr_list *) list, index);
}

toy_str_list *str_list_alloc(const char *str)
{
    assert(offsetof(toy_str_list, str) == offsetof(toy_ptr_list, ptr));
    return (toy_str_list *) ptr_list_alloc((void *) str);
}

toy_str_list *str_list_concat(toy_str_list *orig, toy_str_list *new_list)
{
    assert(offsetof(toy_str_list, next) == offsetof(toy_ptr_list, next));
    return (toy_str_list *) ptr_list_concat((toy_ptr_list *) orig, (toy_ptr_list *) new_list);
}

toy_str_list *str_list_append(toy_str_list *list, toy_str new_item)
{
    assert(offsetof(toy_str_list, next) == offsetof(toy_ptr_list, next));
    assert(offsetof(toy_str_list, str) == offsetof(toy_ptr_list, ptr));
    return (toy_str_list *) ptr_list_append((toy_ptr_list *) list, new_item);
}

size_t str_list_len(const toy_str_list *list)
{
    assert(offsetof(toy_str_list, next) == offsetof(toy_ptr_list, next));
    return ptr_list_len((const toy_ptr_list *) list);
}

void str_list_free(toy_str_list *str_list)
{
    ptr_list_free((toy_ptr_list *) str_list);
}

enumeration_result str_list_foreach(toy_str_list *list, toy_str_list_item_callback callback, void *cookie)
{
    return ptr_list_foreach((toy_ptr_list *) list, (ptr_list_item_callback) callback, cookie);
}

enumeration_result str_list_foreach_const(const toy_str_list *list, const_toy_str_list_item_callback callback, void *cookie)
{
    return ptr_list_foreach_const((const toy_ptr_list *) list, (const_ptr_list_item_callback) callback, cookie);
}

#ifndef NDEBUG
static item_callback_result item_valid_callback(void *cookie, size_t index, const toy_str_list *list)
{
    toy_str str = str_list_payload_const(list);
    str_assert_valid(str);
    return CONTINUE_ENUMERATION;
}

void str_list_assert_valid(const toy_str_list *list)
{
    str_list_foreach_const(list, item_valid_callback, NULL);
}
#endif /* ndef NDEBUG */

enumeration_result str_list_visitor_visit_list(str_list_visitor *visitor, toy_str_list *list)
{
    return ptr_list_visitor_visit_list((ptr_list_visitor *) visitor, (toy_ptr_list *) list);
}

enumeration_result const_str_list_visitor_visit_list(const_str_list_visitor *visitor, const toy_str_list *list)
{
    return const_ptr_list_visitor_visit_list((const_ptr_list_visitor *) visitor, (const toy_ptr_list *) list);
}
