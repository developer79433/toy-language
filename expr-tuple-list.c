#include <stdio.h>
#include <assert.h>

#include "mymalloc.h"
#include "generic-list.h"
#include "expr-tuple-list.h"
#include "dump.h"

toy_expr_tuple_list *expr_tuple_list_alloc_ref(toy_expr *first_key, toy_expr *first_value)
{
    toy_expr_tuple_list *tuple_list;
    tuple_list = mymalloc(toy_expr_tuple_list);
    tuple_list->tuple.key = first_key;
    tuple_list->tuple.value = first_value;
    tuple_list->next = NULL;
    return tuple_list;
}

toy_expr_tuple_list *expr_tuple_list_concat(toy_expr_tuple_list *orig, toy_expr_tuple_list *new_list)
{
    assert(offsetof(toy_expr_tuple_list, next) == offsetof(generic_list, next));
    return (toy_expr_tuple_list *) generic_list_concat((generic_list *) orig, (generic_list *) new_list);
}

typedef struct callback_args_struct {
    FILE *f;
    int printed_anything;
} callback_args;

static void dump_callback(void *cookie, const void *item) {
    callback_args *args = (callback_args *) cookie;
    toy_expr_tuple *tuple = (toy_expr_tuple *) item;
    if (args->printed_anything) {
        fputs(", ", args->f);
    } else {
        fputc(' ', args->f);
    }
    dump_expr(args->f, tuple->key);
    fputs(": ", args->f);
    dump_expr(args->f, tuple->value);
    args->printed_anything = 1;
}

void expr_tuple_list_dump(FILE *f, const toy_expr_tuple_list *list)
{
    assert(offsetof(toy_expr_tuple_list, tuple) == offsetof(generic_list, payload));
    fputc('{', f);
    callback_args args = { .printed_anything = 0, .f = f };
    generic_list_foreach_const(list, dump_callback, &args);
    if (args.printed_anything) {
        fputc(' ', f);
    }
    fputc('}', f);
}
