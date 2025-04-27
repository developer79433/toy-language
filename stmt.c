#include "mymalloc.h"
#include "stmt.h"

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

toy_stmt *alloc_stmt(enum toy_stmt_type type)
{
    toy_stmt *stmt;
    stmt = mymalloc(toy_stmt);
    stmt->type = type;
    return stmt;
}

toy_if_arm *alloc_if_arm(toy_expr *condition, toy_block *block)
{
    toy_if_arm *arm;
    arm = mymalloc(toy_if_arm);
    arm->condition = condition;
    block->type = BLOCK_IF_BODY;
    arm->code.stmts = block->stmts;
    arm->next = NULL;
    return arm;
}

toy_var_decl *alloc_var_decl(toy_str name, toy_expr *value)
{
    toy_var_decl *var_decl;
    var_decl = mymalloc(toy_var_decl);
    var_decl->name = name;
    var_decl->value = value;
    var_decl->next = NULL;
    return var_decl;
}

toy_stmt *append_stmt(toy_stmt *orig, toy_stmt *new_stmt)
{
    toy_stmt *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_stmt;
    return orig;
}

toy_var_decl *append_var_decl(toy_var_decl *orig, toy_var_decl *new_var_decl)
{
    toy_var_decl *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_var_decl;
    return orig;
}

toy_if_arm *append_if_arm(toy_if_arm *orig, toy_if_arm *new_arm)
{
    toy_if_arm *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_arm;
    return orig;
}
