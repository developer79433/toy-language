#ifndef TOY_EXPR_LIST_TYPES_H
#define TOY_EXPR_LIST_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_expr_list_struct {
    struct toy_expr_list_struct *next;
    toy_expr *expr;
} toy_expr_list;

typedef item_callback_result (*toy_expr_list_item_callback)(void *cookie, size_t index, toy_expr_list *item);
typedef item_callback_result (*const_toy_expr_list_item_callback)(void *cookie, size_t index, const toy_expr_list *item);

#endif /* TOY_EXPR_LIST_TYPES_H */
