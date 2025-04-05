#ifndef TOY_EXPR_TUPLE_LIST_H
#define TOY_EXPR_TUPLE_LIST_H 1

#include <stdio.h>

#include "toy-expr-tuple-list-types.h"

toy_expr_tuple_list *append_expr_tuple(toy_expr_tuple_list *orig, toy_expr_tuple_list *new_entry);
toy_expr_tuple_list *alloc_expr_tuple_list(toy_expr *first_key, toy_expr *first_value);
void dump_expr_tuple_list(FILE *f, const toy_expr_tuple_list *list);

#endif /* TOY_EXPR_TUPLE_LIST_H */
