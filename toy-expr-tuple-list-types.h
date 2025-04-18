#ifndef TOY_EXPR_TUPLE_LIST_TYPES_H
#define TOY_EXPR_TUPLE_LIST_TYPES_H 1

#include "toy-str-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_expr_tuple_struct {
    toy_expr *key;
    toy_expr *value;
} toy_expr_tuple;

typedef struct toy_expr_tuple_list_struct {
    struct toy_expr_tuple_list_struct *next;
    toy_expr_tuple tuple;
} toy_expr_tuple_list;

#endif /* TOY_EXPR_TUPLE_LIST_TYPES_H */
