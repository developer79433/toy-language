#ifndef TOY_EXPR_H
#define TOY_EXPR_H 1

#include "expr-types.h"

toy_expr *alloc_unary_op_expr(enum toy_expr_type expr_type);
toy_expr *alloc_binary_op_expr(enum toy_expr_type expr_type);
toy_expr *alloc_expr(enum toy_expr_type type);
toy_expr *alloc_expr_literal(enum toy_val_type val_type);
toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *body);

#endif /* TOY_EXPR_H */
