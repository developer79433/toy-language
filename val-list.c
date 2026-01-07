#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "buf-list.h"
#include "mymalloc.h"
#include "val.h"
#include "val-list.h"
#include "errors.h"
#include "log.h"

typedef struct val_dump_cb_args_struct {
    toy_bool printed_anything;
} val_dump_cb_args;

static item_callback_result val_dump_callback(void *cookie, size_t index, const toy_val_list *item)
{
    val_dump_cb_args *args = (val_dump_cb_args *) cookie;
    const toy_val *val = val_list_payload_const(item);
    if (args->printed_anything) {
        log_puts(", ");
    } else {
        log_putc(' ');
    }
    val_dump(val, 1);
    args->printed_anything = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void val_list_dump(const toy_val_list *list)
{
    log_putc('[');
    val_dump_cb_args args = { .printed_anything = TOY_FALSE };
    enumeration_result res = val_list_foreach_const(list, val_dump_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
    if (args.printed_anything) {
        log_putc(' ');
    }
    log_putc(']');
}

size_t val_list_len(const toy_val_list *list)
{
    assert(offsetof(toy_val_list, next) == offsetof(toy_buf_list, next));
    return buf_list_len((const toy_buf_list *) list);
}

toy_val *val_list_index(toy_val_list *list, size_t index)
{
    assert(offsetof(toy_val_list, val) == offsetof(toy_buf_list, c));
    toy_val *value = buf_list_index((toy_buf_list *) list, index);
    val_assert_valid(value);
    return value;
}

const toy_val *val_list_index_const(const toy_val_list *list, size_t index)
{
    assert(offsetof(toy_val_list, val) == offsetof(toy_buf_list, c));
    const toy_val *value = buf_list_index_const((const toy_buf_list *) list, index);
    val_assert_valid(value);
    return value;
}

toy_val_list *val_list_concat(toy_val_list *orig, toy_val_list *new_list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(toy_buf_list, next));
    return (toy_val_list *) buf_list_concat((toy_buf_list *) orig, (toy_buf_list *) new_list);
}

toy_val_list *val_list_alloc(const toy_val *first_elem)
{
    assert(offsetof(toy_val_list, val) == offsetof(toy_buf_list, c));
    toy_val_list *val_list = (toy_val_list *) buf_list_alloc(first_elem, sizeof(*first_elem));
    assert(0 == memcmp(&val_list->val, first_elem, sizeof(*first_elem)));
    assert(val_list->next == NULL);
    return val_list;
}

toy_val_list *val_list_append(toy_val_list *list, const toy_val *new_item)
{
    assert(offsetof(toy_val_list, val) == offsetof(toy_buf_list, c));
    assert(offsetof(toy_expr_list, next) == offsetof(toy_buf_list, next));
    val_assert_valid(new_item);
    return (toy_val_list *) buf_list_append((toy_buf_list *) list, new_item, sizeof(*new_item));
}

enumeration_result val_list_foreach(toy_val_list *list, toy_val_list_item_callback callback, void *cookie)
{
    assert(offsetof(toy_val_list, val) == offsetof(toy_buf_list, c));
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);
}

enumeration_result val_list_foreach_const(const toy_val_list *list, const_toy_val_list_item_callback callback, void *cookie)
{
    assert(offsetof(toy_val_list, val) == offsetof(toy_buf_list, c));
    return buf_list_foreach_const((const toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);
}

#ifndef NDEBUG
void val_list_assert_valid(const toy_val_list *list)
{
    for (const toy_val_list *cur = list; cur; cur = cur->next) {
        val_assert_valid(&cur->val);
    }
}
#endif /* NDEBUG */

void val_list_free(toy_val_list *list)
{
#ifndef NDEBUG
    val_list_assert_valid(list);
#endif /* NDEBUG */
    /* FIXME: We cannot do this, because we don't know whether we own our storage or not */
#if 0
    for (const toy_val_list *cur = list; cur; cur = cur->next) {
        toy_val_free(cur->val);
    }
#endif
    buf_list_free((toy_buf_list *) list);
}

toy_val *val_list_payload(toy_val_list *list)
{
    toy_val *value = buf_list_payload((toy_buf_list *) list);
    val_assert_valid(value);
    return value;
}

const void *val_list_payload_const(const toy_val_list *list)
{
    const toy_val *value = buf_list_payload_const((const toy_buf_list *) list);
    val_assert_valid(value);
    return value;
}

void val_list_set_payload(toy_val_list *list, const toy_val *value)
{
    val_assert_valid(value);
    return buf_list_payload_set((toy_buf_list *) list, value, sizeof(*value));
}

toy_bool val_list_all_match(const toy_val_list *list, toy_val_list_filter_func filter, void *cookie)
{
    return buf_list_all_match((const toy_buf_list *) list, (buf_list_filter_func) filter, cookie);
}

toy_bool val_list_none_match(const toy_val_list *list, toy_val_list_filter_func filter, void *cookie)
{
    return buf_list_none_match((const toy_buf_list *) list, (buf_list_filter_func) filter, cookie);
}
