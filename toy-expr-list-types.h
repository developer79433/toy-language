#ifndef TOY_EXPR_LIST_TYPES_H
#define TOY_EXPR_LIST_TYPES_H 1

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_expr_list_struct {
    struct toy_expr_list_struct *next;
    toy_expr *expr;
} toy_expr_list;

#endif /* TOY_EXPR_LIST_TYPES_H */
