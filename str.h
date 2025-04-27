#ifndef TOY_STR_H
#define TOY_STR_H 1

#include <stdio.h>

#include "bool-types.h"
#include "str-types.h"

void print_str(FILE *f, const toy_str str);
void dump_str(FILE *f, const toy_str str);
toy_bool toy_str_equal(const toy_str s1, const toy_str s2);
toy_bool toy_str_nequal(const toy_str s1, const toy_str s2);
toy_bool toy_str_equal_nocase(const toy_str s1, const toy_str s2);
toy_bool toy_str_nequal_nocase(const toy_str s1, const toy_str s2);
void str_assert_valid(toy_str str);

#endif /* TOY_STR_H */
