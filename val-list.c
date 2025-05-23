#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "buf-list.h"
#include "mymalloc.h"
#include "val.h"
#include "val-list.h"
#include "errors.h"

void val_list_dump(FILE *f, const toy_val_list *list)
{
    int printed_anything = 0;
    fputc('[', f);
    if (list) {
        /* TODO: Use val_list_foreach_const */
        for (const toy_val_list *cur = list; cur; cur = cur->next) {
            if (printed_anything) {
                fputs(", ", f);
            } else {
                fputc(' ', f);
            }
            val_dump(f, &cur->val);
            printed_anything = 1;
        }
        if (printed_anything) {
            fputc(' ', f);
        }
    }
    fputc(']', f);
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
