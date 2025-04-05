#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "generic-list.h"
#include "mymalloc.h"
#include "toy-val.h"
#include "toy-val-list.h"
#include "errors.h"

void dump_val_list(FILE *f, toy_val_list *list)
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
    void *result = generic_list_index((generic_list *) list, index);
    if (result) {
        return result;
    } else {
        invalid_val_list_index(list, index);
    }
    return NULL;
}

toy_val_list *append_val_list(toy_val_list *orig, toy_val_list *new_list)
{
    assert(offsetof(toy_expr_list, next) == offsetof(generic_list, next));
    return (toy_val_list *) generic_list_append((generic_list *) orig, (generic_list *) new_list);
}

toy_val_list *alloc_val_list(toy_val *first_elem)
{
    toy_val_list *list;
    list = mymalloc(toy_val_list);
    list->val = first_elem;
    list->next = NULL;
    return list;
}

toy_val_list *alloc_val_list_own(toy_val *first_elem)
{
    toy_val_list *list;
    list = (toy_val_list *) malloc(sizeof(toy_val_list) + sizeof(toy_val));
    list->val = (toy_val *) (list + 1);
    memcpy(list->val, first_elem, sizeof(toy_val));
    list->next = NULL;
    return list;
}

toy_val_list *append_val_list_own(toy_val_list *orig, toy_val *new_item)
{
    toy_val_list *list;
    list = (toy_val_list *) malloc(sizeof(toy_val_list) + sizeof(toy_val));
    list->val = (toy_val *) (list + 1);
    memcpy(list->val, new_item, sizeof(toy_val));
    list->next = NULL;
    return append_val_list(orig, list);
}
