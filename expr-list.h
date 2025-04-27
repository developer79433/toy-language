#ifndef TOY_EXPR_LIST_H
#define TOY_EXPR_LIST_H 1

#include <stddef.h>
#include <stdio.h>

#include "expr-list-types.h"

void expr_list_dump(FILE *f, toy_expr_list *list);
toy_expr_list *expr_list_alloc_ref(toy_expr *first_elem);
toy_expr_list *expr_list_alloc_own(toy_expr *first_elem);
size_t expr_list_len(const toy_expr_list *args);
toy_expr_list *expr_list_append_ref(toy_expr_list *list, toy_expr *new_expr);
toy_expr_list *expr_list_append_own(toy_expr_list *list, toy_expr *new_expr);
toy_expr_list *expr_list_concat(toy_expr_list *orig, toy_expr_list *new_list);

#endif /* TOY_EXPR_LIST_H */
