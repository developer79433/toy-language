#include <assert.h>
#include <string.h>

#include "mymalloc.h"
#include "dump.h"
#include "expr.h"
#include "stmt.h"
#include "str.h"
#include "generic-list.h"
#include "buf-list.h"
#include "var-decl.h"

static const char *toy_stmt_type_names[] = {
    "block statement",
    "break statement",
    "continue statement",
    "expression statement",
    "for loop",
    "function declaration",
    "if statement",
    "null statement",
    "return statement",
    "variable declaration",
    "while loop"
};

const char *toy_stmt_type_name(enum toy_stmt_type stmt_type)
{
    return toy_stmt_type_names[stmt_type];
}

toy_stmt *stmt_alloc(enum toy_stmt_type type)
{
    toy_stmt *stmt;
    stmt = mymalloc(toy_stmt);
    stmt->type = type;
    return stmt;
}

toy_stmt_list *stmt_list_alloc(toy_stmt *stmt)
{
    return (toy_stmt_list *) buf_list_alloc(stmt, sizeof(*stmt));
}

toy_stmt_list *stmt_list_concat(toy_stmt_list *list, toy_stmt_list *new_list)
{
    return (toy_stmt_list *) buf_list_concat((toy_buf_list *) list, (toy_buf_list *) new_list);
}

toy_stmt_list *stmt_list_append(toy_stmt_list *list, toy_stmt *stmt)
{
    return (toy_stmt_list *) buf_list_append((toy_buf_list *) list, stmt, sizeof(*stmt));
}

toy_if_arm_list *if_arm_list_alloc(toy_expr *condition, toy_block *block)
{
    toy_if_arm_list *arm_list;
    arm_list = mymalloc(toy_if_arm_list);
    arm_list->arm.condition = condition;
    arm_list->arm.code.stmts = block->stmts;
    arm_list->next = NULL;
    return arm_list;
}

toy_if_arm_list *if_arm_list_concat(toy_if_arm_list *list, toy_if_arm_list *new_list)
{
    return (toy_if_arm_list *) buf_list_concat((toy_buf_list *) list, (toy_buf_list *) new_list);
}
