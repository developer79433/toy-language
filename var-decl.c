#include <stdlib.h>
#include <assert.h>

#include "mymalloc.h"
#include "str.h"
#include "expr.h"
#include "var-decl.h"
#include "log.h"

toy_var_decl *var_decl_alloc(toy_str name, toy_expr *value)
{
    toy_var_decl *decl = mymalloc(toy_var_decl);
    decl->name = name;
    decl->value = value;
    return decl;
}

void var_decl_assert_valid(const toy_var_decl *decl)
{
    str_assert_valid(decl->name);
    expr_assert_valid(decl->value);
}

void var_decl_dump(const toy_var_decl *decl)
{
    log_puts(decl->name);
    if (decl->value) {
        log_puts(" = ");
        expr_dump(decl->value);
    }
}

void var_decl_free(toy_var_decl *decl)
{
    free(decl);
}
