#ifndef TOY_EXPR_H
#define TOY_EXPR_H 1

#include "bool-types.h"
#include "expr-types.h"
#include "stmt-types.h"
#include "str-list-types.h"

toy_bool toy_expr_equal(const toy_expr *expr1, const toy_expr *expr2);
toy_expr *alloc_unary_op_expr(toy_expr_type expr_type);
toy_expr *alloc_binary_op_expr(toy_expr_type expr_type);
toy_expr *alloc_expr(toy_expr_type type);
toy_expr *alloc_expr_literal(toy_val_type val_type);
toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *body);
const char *toy_expr_type_name(toy_expr_type expr_type);
#ifndef NDEBUG
void expr_type_assert_valid(toy_expr_type type);
void expr_assert_valid(const toy_expr *expr);
#endif /* NDEBUG */

#endif /* TOY_EXPR_H */
