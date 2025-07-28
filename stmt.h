#ifndef TOY_STMT_H
#define TOY_STMT_H 1

#include <stdio.h>

#include "str-types.h"
#include "expr-types.h"
#include "stmt-types.h"

toy_stmt *stmt_alloc(toy_stmt_type type);
toy_stmt *func_decl_stmt_alloc(toy_str name, toy_str_list *param_names, toy_block *block);
void func_decl_stmt_dump(FILE *f, const toy_func_decl_stmt *func_decl);
toy_stmt *var_decl_stmt_alloc(toy_var_decl_list *var_decl_list);
void var_decl_dump(FILE *f, const toy_var_decl *var_decl);
const char *stmt_type_name(toy_stmt_type stmt_type);

#endif /* TOY_STMT_H */
