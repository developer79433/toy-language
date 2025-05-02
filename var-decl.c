#include <stdlib.h>

#include "mymalloc.h"
#include "str.h"
#include "expr.h"
#include "var-decl.h"
#include "dump.h"

toy_var_decl *var_decl_alloc(toy_str name, toy_expr *value)
{
    toy_var_decl *decl = mymalloc(toy_var_decl);
    decl->name = name;
    decl->value = value;
    return decl;
}

void var_decl_dump(FILE *f, const toy_var_decl *decl)
{
    fputs(decl->name, f);
    if (decl->value) {
        fputs(" = ", f);
        dump_expr(f, decl->value);
    }
}

void var_decl_free(toy_var_decl *decl)
{
    free(decl);
}
