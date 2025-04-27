#ifndef TOY_CONSTANTS_H
#define TOY_CONSTANTS_H 1

#include "toy-str-types.h"
#include "toy-val-types.h"
#include "expr-types.h"

typedef struct predefined_constant_struct {
    toy_str name;
    const toy_val *value;
} predefined_constant;

extern const toy_val null_val;
extern const toy_val true_val;
extern const toy_val false_val;
extern const toy_expr null_expr;

const toy_val *lookup_predefined_constant(toy_str name);

#endif /* TOY_CONSTANTS_H */
