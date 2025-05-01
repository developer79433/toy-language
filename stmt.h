#ifndef TOY_STMT_H
#define TOY_STMT_H 1

#include <stdio.h>

#include "str-types.h"
#include "expr-types.h"
#include "stmt-types.h"

toy_stmt *stmt_alloc(enum toy_stmt_type type);
toy_stmt *stmt_append(toy_stmt *orig, toy_stmt *new_stmt);
const char *toy_stmt_type_name(enum toy_stmt_type stmt_type);

toy_if_arm_list *if_arm_list_alloc(toy_expr *condition, toy_block *code);
toy_if_arm_list *if_arm_list_append(toy_if_arm_list *orig, toy_if_arm_list *new_arm);

#endif /* TOY_STMT_H */
