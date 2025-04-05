#ifndef TOY_EXPR_LIST_H
#define TOY_EXPR_LIST_H 1

#include <stddef.h>
#include <stdio.h>

#include "toy-expr-list-types.h"

void dump_expr_list(FILE *f, toy_expr_list *list);
toy_expr_list *alloc_expr_list(toy_expr *first_elem);
size_t expr_list_len(const toy_expr_list *args);
toy_expr_list *append_expr_list(toy_expr_list *orig, toy_expr_list *new_item);

#endif /* TOY_EXPR_LIST_H */
