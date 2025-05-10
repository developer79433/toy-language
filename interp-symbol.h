#ifndef TOY_INTERP_SYMBOL_H
#define TOY_INTERP_SYMBOL_H 1

#include "str-types.h"
#include "expr-types.h"
#include "interp-types.h"

void create_variable_expr(toy_interp *interp, const toy_str name, const toy_expr *expr);
void create_variable_value(toy_interp *interp, const toy_str name, const toy_val *value);
void set_variable_value(toy_interp *interp, const toy_str name, const toy_val *value);
void op_assign(toy_interp *interp, toy_val *result, toy_str name, toy_expr *value);
void create_function(toy_interp *interp, const toy_function *def);
get_result lookup_identifier(toy_interp *interp, toy_val *result, const toy_str name);

#endif /* TOY_INTERP_SYMBOL_H */
