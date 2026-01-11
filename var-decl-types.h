#ifndef TOY_VAR_DECL_TYPES_H
#define TOY_VAR_DECL_TYPES_H 1

#include <stddef.h>

#include "str-types.h"
#include "expr-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_var_decl_struct {
    toy_str name;
    toy_expr *value;
    size_t decl_index; /* offset in parent block's list of declarations */
} toy_var_decl;

#endif /* TOY_VAR_DECL_TYPES_H */
