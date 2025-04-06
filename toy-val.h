#ifndef TOY_VAL_H
#define TOY_VAL_H 1

#include <stdio.h>

#include "toy-bool-types.h"
#include "toy-val-types.h"

void dump_val(FILE *f, const toy_val *val);
toy_bool toy_vals_equal(const toy_val *val1, const toy_val *val2);
toy_bool toy_vals_nequal(const toy_val *val1, const toy_val *val2);
toy_bool toy_val_gt(const toy_val *val1, const toy_val *val2);
toy_bool toy_val_gte(const toy_val *val1, const toy_val *val2);
toy_bool toy_val_lt(const toy_val *val1, const toy_val *val2);
toy_bool toy_val_lte(const toy_val *val1, const toy_val *val2);
void val_assert_valid(toy_val *val);

#endif /* TOY_VAL_H */
