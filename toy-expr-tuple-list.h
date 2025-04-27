#ifndef TOY_EXPR_TUPLE_LIST_H
#define TOY_EXPR_TUPLE_LIST_H 1

#include <stdio.h>

#include "toy-expr-tuple-list-types.h"

toy_expr_tuple_list *expr_tuple_list_concat(toy_expr_tuple_list *orig, toy_expr_tuple_list *new_entry);
toy_expr_tuple_list *expr_tuple_list_alloc_ref(toy_expr *first_key, toy_expr *first_value);
void expr_tuple_list_dump(FILE *f, const toy_expr_tuple_list *list);

#endif /* TOY_EXPR_TUPLE_LIST_H */
