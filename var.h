#ifndef TOY_VAR_H
#define TOY_VAR_H 1

#include "bool-types.h"
#include "var-types.h"
#include "val-types.h"

void var_init(toy_var *var);
toy_var *var_alloc_ref(toy_val *val);
toy_var *var_alloc_copy(const toy_val *val);
toy_var *var_alloc_array(size_t count);
toy_var *var_ref(toy_var *var);
toy_val *var_get(toy_var *var);
const toy_val *var_get_const(const toy_var *var);
void var_set(toy_var *var, toy_val *new_val);
void var_set_copy(toy_var *var, const toy_val *new_val);
void var_dump(const toy_var *var, toy_bool verbose);
void var_assert_valid(const toy_var *var);
void var_array_assert_valid(const toy_var *vars, size_t count);
void var_array_dump(const toy_var *vars, size_t size, toy_bool verbose);
void var_free(toy_var *var);
void var_array_free(toy_var *vars, size_t size);

#endif /* TOY_VAR_H */
