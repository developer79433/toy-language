#ifndef TOY_VAL_H
#define TOY_VAL_H 1

#include "bool-types.h"
#include "val-types.h"
#include "str-list-types.h"
#include "function-types.h"

toy_val *val_alloc(void);
void val_dump(const toy_val *val, toy_bool verbose);
void val_array_dump(const toy_val *vals, size_t size);
toy_bool vals_equal(const toy_val *val1, const toy_val *val2);
toy_bool vals_nequal(const toy_val *val1, const toy_val *val2);
toy_bool num_gt(toy_num num1, toy_num num2);
toy_bool num_gte(toy_num num1, toy_num num2);
toy_bool num_lt(toy_num num1, toy_num num2);
toy_bool num_lte(toy_num num1, toy_num num2);
toy_bool val_gt(const toy_val *val1, const toy_val *val2);
toy_bool val_gte(const toy_val *val1, const toy_val *val2);
toy_bool val_lt(const toy_val *val1, const toy_val *val2);
toy_bool val_lte(const toy_val *val1, const toy_val *val2);
#ifndef NDEBUG
void val_assert_valid(const toy_val *val);
void val_assert_valid_or_null(const toy_val *val);
void assert_vals_equal(const toy_val *val1, const toy_val *val2);
#else /* ndef NDEBUG */
#define val_assert_valid(val) do {} while (0)
#define val_assert_valid_or_null(val) do {} while (0)
#define assert_vals_equal(val1, val2) do {} while (0)
#endif /* NDEBUG */
const char *val_type_name(toy_val_type val_type);
void val_free(toy_val *val);
toy_bool val_truthy(const toy_val *val);
toy_bool val_falsey(const toy_val *val);
toy_val *val_alloc_func_decl(toy_str_list *formalparams, toy_block *body);

#endif /* TOY_VAL_H */
