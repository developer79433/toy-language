#ifndef TOY_STMT_H
#define TOY_STMT_H 1

#include "str-types.h"
#include "expr-types.h"
#include "stmt-types.h"

toy_stmt *alloc_stmt(enum toy_stmt_type type);
toy_if_arm *alloc_if_arm(toy_expr *condition, toy_block *code);
toy_var_decl *alloc_var_decl(toy_str name, toy_expr *value);
toy_stmt *append_stmt(toy_stmt *orig, toy_stmt *new_stmt);
toy_var_decl *append_var_decl(toy_var_decl *orig, toy_var_decl *new_var_decl);
toy_if_arm *append_if_arm(toy_if_arm *orig, toy_if_arm *new_arm);
const char *toy_stmt_type_name(enum toy_stmt_type stmt_type);

#endif /* TOY_STMT_H */
