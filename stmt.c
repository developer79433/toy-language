#include "mymalloc.h"
#include "stmt.h"
#include "generic-list.h"

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

toy_if_arm *if_arm_alloc(toy_expr *condition, toy_block *block)
{
    toy_if_arm *arm;
    arm = mymalloc(toy_if_arm);
    arm->condition = condition;
    arm->code.stmts = block->stmts;
    arm->next = NULL;
    return arm;
}

toy_var_decl_list *var_decl_list_alloc_ref(toy_str name, toy_expr *value)
{
    /* TODO: USe generic_list_alloc */
    toy_var_decl_list *var_decl;
    var_decl = mymalloc(toy_var_decl_list);
    var_decl->name = name;
    var_decl->value = value;
    var_decl->next = NULL;
    return var_decl;
}

toy_stmt *stmt_append(toy_stmt *orig, toy_stmt *new_stmt)
{
    toy_stmt *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_stmt;
    return orig;
}

toy_var_decl_list *var_decl_list_concat(toy_var_decl_list *orig, toy_var_decl_list *new_var_decl)
{
    toy_var_decl_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_var_decl;
    return orig;
}

toy_if_arm *if_arm_append(toy_if_arm *orig, toy_if_arm *new_arm)
{
    toy_if_arm *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_arm;
    return orig;
}
