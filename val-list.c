#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "generic-list.h"
#include "mymalloc.h"
#include "val.h"
#include "val-list.h"
#include "errors.h"

void val_list_dump(FILE *f, toy_val_list *list)
{
    int printed_anything = 0;
    fputc('[', f);
    if (list) {
        for (toy_val_list *cur = list; cur; cur = cur->next) {
            if (printed_anything) {
                fputs(", ", f);
            } else {
                fputc(' ', f);
            }
            dump_val(f, cur->val);
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
    assert(offsetof(toy_val_list, next) == offsetof(generic_list, next));
    return generic_list_len((const generic_list *) list);
}

toy_val *val_list_index(toy_val_list *list, toy_num index)
{
    assert(offsetof(toy_val_list, val) == offsetof(generic_list, payload));
    toy_val *result = (toy_val *) generic_list_index((generic_list *) list, index);
    if (result) {
        return result;
    } else {
        invalid_val_list_index(list, index);
    }
    return result;
}

toy_val_list *val_list_concat(toy_val_list *orig, toy_val_list *new_list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_val_list *) generic_list_concat((generic_list *) orig, (generic_list *) new_list);
}

toy_val_list *val_list_alloc_ref(toy_val *first_elem)
{
    assert(offsetof(toy_val_list, val) == offsetof(generic_list, payload));
    return (toy_val_list *) generic_list_alloc_ref(first_elem);
}

toy_val_list *val_list_alloc_own(toy_val *first_elem)
{
    assert(offsetof(toy_val_list, val) == offsetof(generic_list, payload));
    return (toy_val_list *) generic_list_alloc_own(first_elem, sizeof(*first_elem));
}

toy_val_list *val_list_append_ref(toy_val_list *orig, toy_val *new_item)
{
    assert(offsetof(toy_val_list, val) == offsetof(generic_list, payload));
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_val_list *) generic_list_append_ref((generic_list *) orig, new_item);
}

toy_val_list *val_list_append_own(toy_val_list *orig, toy_val *new_item)
{
    assert(offsetof(toy_val_list, val) == offsetof(generic_list, payload));
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_val_list *) generic_list_append_own((generic_list *) orig, new_item, sizeof(*new_item));
}

void val_list_foreach(toy_val_list *list, val_list_item_callback callback, void *cookie)
{
    assert(offsetof(toy_val_list, val) == offsetof(generic_list, payload));
    generic_list_foreach(list, (list_item_callback) callback, cookie);
}

void val_list_foreach_const(const toy_val_list *list, const_val_list_item_callback callback, void *cookie)
{
    assert(offsetof(toy_val_list, val) == offsetof(generic_list, payload));
    generic_list_foreach_const(list, (const_list_item_callback) callback, cookie);
}

void val_list_assert_valid(const toy_val_list *val_list)
{
    assert(val_list);
    for (const toy_val_list *cur = val_list; cur; cur = cur->next) {
        val_assert_valid(cur->val);
    }
}
