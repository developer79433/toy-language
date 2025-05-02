#ifndef TOY_VAL_H
#define TOY_VAL_H 1

#include <stdio.h>

#include "bool-types.h"
#include "val-types.h"

void dump_val(FILE *f, const toy_val *val);
/* TODO: Remove 'toy_' prefix */
toy_bool toy_vals_equal(const toy_val *val1, const toy_val *val2);
toy_bool toy_vals_nequal(const toy_val *val1, const toy_val *val2);
toy_bool toy_val_gt(const toy_val *val1, const toy_val *val2);
toy_bool toy_val_gte(const toy_val *val1, const toy_val *val2);
toy_bool toy_val_lt(const toy_val *val1, const toy_val *val2);
toy_bool toy_val_lte(const toy_val *val1, const toy_val *val2);
void val_assert_valid(const toy_val *val);
const char *toy_val_type_name(enum toy_val_type val_type);
void toy_val_free(toy_val *val);

#endif /* TOY_VAL_H */
