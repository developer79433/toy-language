#ifndef TOY_FUCTIONS_H
#define TOY_FUCTIONS_H 1

#include <stdio.h>

#include "toy-str-types.h"
#include "toy-val-types.h"
#include "function-types.h"

extern const toy_str_list INFINITE_PARAMS;

const toy_func_def *lookup_predefined_function_name(const toy_str name);
const toy_func_def *lookup_predefined_function_addr(predefined_func_addr func_addr);
void dump_function(FILE *f, const toy_func_def *def);
const char *block_type_name(enum block_type btype);
void func_assert_valid(const toy_func_def *func_def);

#endif /* TOY_FUCTIONS_H */
