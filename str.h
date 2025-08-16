#ifndef TOY_STR_H
#define TOY_STR_H 1

#include "bool-types.h"
#include "str-types.h"

void print_str(const toy_str str);
/* TODO: Should be str_dump */
void dump_str(const toy_str str);
toy_bool toy_str_equal(const toy_str s1, const toy_str s2);
toy_bool toy_str_nequal(const toy_str s1, const toy_str s2);
toy_bool toy_str_equal_nocase(const toy_str s1, const toy_str s2);
toy_bool toy_str_nequal_nocase(const toy_str s1, const toy_str s2);
void str_assert_valid(toy_str str);
void str_backslash_decode(toy_str dst, const toy_str src, const toy_str src_end);
toy_str toy_str_concat_alloc(toy_str str1, toy_str str2);
void str_free(toy_str str);

#endif /* TOY_STR_H */
