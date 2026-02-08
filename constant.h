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

const predefined_constant *constant_get(toy_str name);
const toy_val *constant_get_val(toy_str name);
toy_bool is_constant(toy_str name);
void constant_dump(const predefined_constant *predef_const);
void constant_assert_valid(const predefined_constant *predef_const);

#endif /* TOY_CONSTANTS_H */
