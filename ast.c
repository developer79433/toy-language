#include <string.h>
#include <malloc.h>

#include "ast.h"

#define mymalloc(the_type) ((the_type *) malloc(sizeof(the_type)))

toy_stmt *alloc_stmt(enum toy_stmt_type type)
{
    toy_stmt *stmt;
    stmt = mymalloc(toy_stmt);
    stmt->type = type;
    return stmt;
}

toy_if_arm *alloc_if_arm(toy_expr *condition, toy_stmt *code)
{
    toy_if_arm *arm;
    arm = mymalloc(toy_if_arm);
    arm->condition = condition;
    arm->code = code;
    arm->next = NULL;
    return arm;
}

toy_str_list *alloc_str_list(const char *str)
{
    size_t size = sizeof(toy_str_list) + strlen(str) + 1;
    void *mem = malloc(size);
    toy_str_list *list = (toy_str_list *) mem;
    list->next = NULL;
    list->str = (char *) (list + 1);
    strcpy(list->str, str);
    return list;
}

toy_var_decl *alloc_var_decl()
{
    toy_var_decl *var_decl;
    var_decl = mymalloc(toy_var_decl);
    var_decl->next = NULL;
    var_decl->name = NULL;
    var_decl->value = NULL;
    return var_decl;
}

toy_expr *alloc_expr(enum toy_expr_type type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = type;
    return expr;
}

toy_expr *alloc_expr_func_decl(void *formalparams, toy_stmt *code)
{
    toy_expr *expr;
    expr = (toy_expr *) malloc(sizeof(toy_expr) + sizeof(toy_func_expr));
    expr->func_decl = (toy_func_expr *) (expr + 1);
    expr->func_decl->def.name = ""; /* TODO: generated unique name */
    expr->func_decl->def.param_names = formalparams;
    expr->func_decl->def.code = code;
    return expr;
}

toy_list *alloc_toy_list(toy_expr *first_elem)
{
    toy_list *list;
    list = mymalloc(toy_list);
    list->expr = first_elem;
    list->next = NULL;
    return list;
}

toy_stmt *append_stmt(toy_stmt *orig, toy_stmt *new)
{
    toy_stmt *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_var_decl *append_var_decl(toy_var_decl *orig, toy_var_decl *new)
{
    toy_var_decl *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_if_arm *append_if_arm(toy_if_arm *orig, toy_if_arm *new)
{
    toy_if_arm *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_str_list *append_str_list(toy_str_list *orig, toy_str_list *new)
{
    toy_str_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_list *append_list(toy_list *orig, toy_list *new)
{
    toy_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

static const char *toy_expr_type_names[] = {
    "logical and",
    "assignment",
    "boolean",
    "division",
    "equal to",
    "function call",
    "function declaration"
    "greater than",
    "greater than or equal to",
    "identifier",
    "in list",
    "list",
    "less than",
    "less than or equal to",
    "map",
    "subtraction",
    "multiplication",
    "not equal to",
    "logical inverse",
    "numeric",
    "logical or",
    "addition",
    "string",
    "unary negation"
};

const char *toy_expr_type_name(enum toy_expr_type expr_type)
{
    return toy_expr_type_names[expr_type];
}

static const char *toy_stmt_type_names[] = {
    "expression statement",
    "for loop",
    "function declaration",
    "null statement",
    "if statement",
    "variable declaration",
    "while loop"
};

const char *toy_stmt_type_name(enum toy_stmt_type stmt_type)
{
    return toy_stmt_type_names[stmt_type];
}
