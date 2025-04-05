#include <assert.h>
#include <string.h>

#include "mymalloc.h"
#include "ast.h"
#include "toy-val-list.h"
#include "toy-map.h"
#include "dump.h"

static const char *toy_expr_type_names[] = {
    "logical and",
    "assignment",
    "collection lookup",
    "comma",
    "division",
    "equal to",
    "exponentiation",
    "field reference",
    "function call",
    "greater than",
    "greater than or equal to",
    "identifier",
    "in list",
    "list",
    "literal",
    "less than",
    "less than or equal to",
    "map",
    "method call",
    "subtraction",
    "modulus",
    "multiplication",
    "not equal to",
    "logical negation",
    "logical or",
    "addition",
    "postfix decrement",
    "postfix increment",
    "prefix decrement",
    "prefix increment",
    "unary negation"
};

const char *toy_expr_type_name(enum toy_expr_type expr_type)
{
    return toy_expr_type_names[expr_type];
}

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

toy_expr *alloc_unary_op_expr(enum toy_expr_type expr_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    expr->unary_op.arg = NULL;
    return expr;
}

toy_expr *alloc_binary_op_expr(enum toy_expr_type expr_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    expr->binary_op.arg1 = NULL;
    expr->binary_op.arg2 = NULL;
    return expr;
}

toy_expr *alloc_expr(enum toy_expr_type expr_type)
{
    toy_expr *expr;
    assert(EXPR_LITERAL != expr_type);
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    return expr;
}

toy_expr *alloc_expr_literal(enum toy_val_type val_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_LITERAL;
    expr->val.type = val_type;
    return expr;
}

toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *body)
{
    toy_expr *expr;
    expr = (toy_expr *) malloc(sizeof(toy_expr) + sizeof(toy_func_def));
    expr->type = EXPR_LITERAL;
    expr->val.type = VAL_FUNC;
    expr->val.func = (toy_func_def *) (expr + 1);
    expr->val.func->type = FUNC_USER_DECLARED;
    expr->val.func->name = ""; /* TODO: generated unique name */
    expr->val.func->param_names = formalparams;
    expr->val.func->code.stmts = body->stmts;
    return expr;
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
