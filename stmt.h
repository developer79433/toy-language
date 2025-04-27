#ifndef TOY_STMT_H
#define TOY_STMT_H 1

#include "str-types.h"
#include "expr-types.h"
#include "stmt-types.h"

toy_stmt *stmt_alloc(enum toy_stmt_type type);
toy_stmt *stmt_append(toy_stmt *orig, toy_stmt *new_stmt);
const char *toy_stmt_type_name(enum toy_stmt_type stmt_type);
toy_var_decl *var_decl_alloc(toy_str name, toy_expr *value);
toy_var_decl *var_decl_append(toy_var_decl *orig, toy_var_decl *new_var_decl);
toy_if_arm *if_arm_alloc(toy_expr *condition, toy_block *code);
toy_if_arm *if_arm_append(toy_if_arm *orig, toy_if_arm *new_arm);

#endif /* TOY_STMT_H */
