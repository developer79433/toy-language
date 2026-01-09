#ifndef TOY_VAR_H
#define TOY_VAR_H 1

#include "var-types.h"
#include "val-types.h"

toy_var *var_alloc(toy_val *val);
toy_var *var_ref(toy_var *var);
void var_free(toy_var *var);

#endif /* TOY_VAR_H */
