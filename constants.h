#ifndef TOY_CONSTANTS_H
#define TOY_CONSTANTS_H 1

#include "bool-types.h"
#include "str-types.h"
#include "val-types.h"
#include "expr-types.h"
#include "constant-types.h"

extern const toy_val null_val;
extern const toy_val true_val;
extern const toy_val false_val;
extern const toy_expr null_expr;

const predefined_constant *lookup_predefined_constant(toy_str name);
toy_bool is_predefined(toy_str name);
void predef_const_dump(const predefined_constant *predef_const);
void predef_const_assert_valid(const predefined_constant *predef_const);

#endif /* TOY_CONSTANTS_H */
