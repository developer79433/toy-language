#include <assert.h>

#include "expr.h"
#include "mymalloc.h"

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
