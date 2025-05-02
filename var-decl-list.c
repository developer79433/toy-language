#include <assert.h>

#include "str.h"
#include "expr.h"
#include "buf-list.h"
#include "var-decl.h"
#include "var-decl-list.h"

toy_var_decl_list *var_decl_list_alloc(toy_var_decl *decl)
{
    assert(offsetof(toy_var_decl_list, decl) == offsetof(toy_buf_list, c));
    assert(offsetof(toy_var_decl_list, next) == offsetof(generic_list, next));
    assert(offsetof(toy_var_decl_list, next) == offsetof(toy_buf_list, next));
    toy_buf_list *buf_list = buf_list_alloc(decl, sizeof(*decl));
    toy_var_decl_list *decl_list = (toy_var_decl_list *) buf_list;
    assert(decl_list->decl.name == decl->name);
    assert(decl_list->decl.value == decl->value);
    return decl_list;
}

void var_decl_list_dump(FILE *f, const toy_var_decl_list *list)
{
    int output_something = 0;
    for (const toy_var_decl_list *decl = list; decl; decl = decl->next) {
        if (output_something) {
            fputs(", ", f);
        }
        var_decl_dump(f, &decl->decl);
        output_something = 1;
    }
}

void var_decl_list_free(toy_var_decl_list *list)
{
    buf_list_free((toy_buf_list *) list);
}

toy_var_decl_list *var_decl_list_concat(toy_var_decl_list *list, toy_var_decl_list *new_list)
{
    assert(offsetof(toy_var_decl_list, next) == offsetof(toy_buf_list, next));
    return (toy_var_decl_list *) buf_list_concat((toy_buf_list *) list, (toy_buf_list *) new_list);
}

toy_var_decl_list *var_decl_list_append(toy_var_decl_list *list, toy_var_decl *new_var_decl)
{
    toy_var_decl_list *new_list = var_decl_list_alloc(new_var_decl);
    assert(new_list->decl.name == new_var_decl->name);
    assert(new_list->decl.value == new_var_decl->value);
    assert(NULL == new_list->next);
    return var_decl_list_concat(list, new_list);
}
