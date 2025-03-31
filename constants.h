#ifndef CONSTANTS_H
#define CONSTANTS_H 1

#include "ast.h"

typedef struct predefined_constant_struct {
    toy_str name;
    const toy_expr *value;
} predefined_constant;

extern const toy_expr null_expr;
extern toy_expr true_expr;
extern toy_expr false_expr;

const toy_expr *lookup_predefined_constant(toy_str name);

#endif /* CONSTANTS_H */
