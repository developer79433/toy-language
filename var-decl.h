#ifndef TOY_VAR_DECL_H
#define TOY_VAR_DECL_H 1

#include <stdio.h>

#include "var-decl-types.h"

toy_var_decl *var_decl_alloc(toy_str name, toy_expr *value);
void var_decl_dump(FILE *f, toy_var_decl *decl);
void var_decl_free(toy_var_decl *decl);

#endif /* TOY_VAR_DECL_H */
