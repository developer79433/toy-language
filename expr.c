#include <assert.h>
#include <string.h>

#include "expr.h"
#include "mymalloc.h"
#include "map-val.h"
#include "function.h"
#include "constants.h"
#include "predef-function.h"
#include "errors.h"
#include "str.h"
#include "str-list.h"
#include "stmt.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "if-arm-list.h"
#include "expr-list.h"

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

const char *toy_expr_type_name(toy_expr_type expr_type)
{
    return toy_expr_type_names[expr_type];
}

toy_expr *alloc_unary_op_expr(toy_expr_type expr_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    expr->unary_op.arg = NULL;
    return expr;
}

toy_expr *alloc_binary_op_expr(toy_expr_type expr_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    expr->binary_op.arg1 = NULL;
    expr->binary_op.arg2 = NULL;
    return expr;
}

toy_expr *alloc_expr(toy_expr_type expr_type)
{
    toy_expr *expr;
    assert(EXPR_LITERAL != expr_type);
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    return expr;
}

toy_expr *alloc_expr_literal(toy_val_type val_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_LITERAL;
    expr->val.type = val_type;
    return expr;
}

toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *block)
{
    toy_expr *expr;
    expr = (toy_expr *) malloc(sizeof(toy_expr) + sizeof(toy_function));
    expr->type = EXPR_LITERAL;
    expr->val.type = VAL_FUNC;
    expr->val.func = (toy_function *) (expr + 1);
    expr->val.func->type = FUNC_USER_DECLARED;
    expr->val.func->name = ""; /* TODO: generated unique name */
    expr->val.func->parent = NULL;
    expr->val.func->code.stmts = block->stmts;
    expr->val.func->param_names = formalparams;
    return expr;
}

toy_expr *alloc_expr_func_call(toy_str id, toy_expr_list *args)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_FUNC_CALL;
    expr->func_call.id = id;
    expr->func_call.args = args;
    return expr;
}

toy_expr *alloc_expr_method_call(toy_str lhs, toy_str method_name, toy_expr_list *args)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_METHOD_CALL;
    expr->method_call.lhs = lhs;
    expr->method_call.method_name = method_name;
    expr->method_call.args = args;
    return expr;
}

toy_bool toy_expr_equal(const toy_expr *expr1, const toy_expr *expr2)
{
    return 0 == memcmp(expr1, expr2, sizeof(*expr1));
}

#ifndef NDEBUG

void expr_type_assert_valid(toy_expr_type type)
{
    assert(type >= 0);
    assert(type <= EXPR_MAX);
}

void expr_assert_valid(const toy_expr *expr)
{
    expr_type_assert_valid(expr->type);
    /* TODO */
}

#endif /* NDEBUG */
