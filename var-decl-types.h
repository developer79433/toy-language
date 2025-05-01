#ifndef TOY_VAR_DECL_TYPES_H
#define TOY_VAR_DECL_TYPES_H 1

#include "str-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_var_decl_struct {
    toy_str name;
    toy_expr *value;
} toy_var_decl;

#endif /* TOY_VAR_DECL_TYPES_H */
