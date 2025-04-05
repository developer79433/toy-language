#include <stddef.h>
#include <string.h>

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
    size_t size;
    for (size = 0; list; list = list->next) {
        size++;
    }
    return size;
}

toy_val *val_list_index(toy_val_list *list, toy_num index)
{
    /* FIXME: inefficient */
    for (size_t i = 0; list; list = list->next, i++) {
        if (i == index) {
            return list->val;
        }
    }
    invalid_val_list_index(list, index);
    return NULL;
}

toy_val_list *append_val_list(toy_val_list *orig, toy_val_list *new_item)
{
    toy_val_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_item;
    return orig;
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
