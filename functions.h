#ifndef TOY_FUCTIONS_H
#define TOY_FUCTIONS_H 1

#include <stdio.h>

#include "toy-str-types.h"
#include "toy-val-types.h"
#include "function-types.h"

extern const toy_str_list INFINITE_PARAMS;

const toy_func_def *lookup_predefined_function(const toy_str name);
void dump_function(FILE *f, const toy_func_def *def);
const char *block_type_name(enum block_type btype);

#endif /* TOY_FUCTIONS_H */
