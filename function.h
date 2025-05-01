#ifndef TOY_FUCTIONS_H
#define TOY_FUCTIONS_H 1

#include <stdio.h>

#include "str-types.h"
#include "val-types.h"
#include "function-types.h"

extern const toy_str_list INFINITE_PARAMS;

const toy_func_def *func_lookup_predef_name(const toy_str name);
const toy_func_def *func_lookup_predef_addr(predefined_func_addr func_addr);
void func_dump(FILE *f, const toy_func_def *def);
void func_assert_valid(const toy_func_def *func_def);
void func_free(toy_func_def *def);

#endif /* TOY_FUCTIONS_H */
