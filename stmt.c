#include <assert.h>
#include <string.h>

#include "mymalloc.h"
#include "dump.h"
#include "expr.h"
#include "stmt.h"
#include "str.h"
#include "generic-list.h"
#include "buf-list.h"
#include "ptr-list.h"
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
    stmt->next = NULL;
    return stmt;
}

toy_stmt *stmt_append(toy_stmt *list, toy_stmt *new_list)
{
    return (toy_stmt *) ptr_list_concat((toy_ptr_list *) list, (toy_ptr_list *) new_list);
}

toy_if_arm_list *if_arm_list_alloc(toy_expr *condition, toy_block *block)
{
    toy_if_arm_list *arm;
    arm = mymalloc(toy_if_arm_list);
    arm->condition = condition;
    arm->code.stmts = block->stmts;
    arm->next = NULL;
    return arm;
}

toy_if_arm_list *if_arm_list_concat(toy_if_arm_list *list, toy_if_arm_list *new_list)
{
    return (toy_if_arm_list *) ptr_list_concat((toy_ptr_list *) list, (toy_ptr_list *) new_list);
}
