#ifndef TOY_VAL_H
#define TOY_VAL_H 1

#include <stdio.h>

#include "bool-types.h"
#include "val-types.h"

void val_dump(FILE *f, const toy_val *val);
toy_bool vals_equal(const toy_val *val1, const toy_val *val2);
toy_bool vals_nequal(const toy_val *val1, const toy_val *val2);
toy_bool val_gt(const toy_val *val1, const toy_val *val2);
toy_bool val_gte(const toy_val *val1, const toy_val *val2);
toy_bool val_lt(const toy_val *val1, const toy_val *val2);
toy_bool val_lte(const toy_val *val1, const toy_val *val2);
#ifndef NDEBUG
void val_assert_valid(const toy_val *val);
void assert_vals_equal(const toy_val *val1, const toy_val *val2);
#endif /* NDEBUG */
const char *val_type_name(toy_val_type val_type);
void val_free(toy_val *val);
void assert_vals_equal(const toy_val *val1, const toy_val *val2);

#endif /* TOY_VAL_H */
