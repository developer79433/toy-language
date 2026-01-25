#ifndef TOY_EXPR_LIST_H
#define TOY_EXPR_LIST_H 1

#include <stddef.h>

#include "generic-list-types.h"
#include "expr-list-types.h"

void expr_list_dump(const toy_expr_list *list, toy_bool include_braces);
toy_expr_list *expr_list_alloc(toy_expr *first_elem);
size_t expr_list_len(const toy_expr_list *args);
toy_expr *expr_list_payload(toy_expr_list *list);
const toy_expr *expr_list_payload_const(const toy_expr_list *list);
void expr_list_set_payload(toy_expr_list *list, toy_expr *expr);
toy_expr_list *expr_list_append(toy_expr_list *list, toy_expr *new_expr);
toy_expr_list *expr_list_concat(toy_expr_list *orig, toy_expr_list *new_list);

#endif /* TOY_EXPR_LIST_H */
