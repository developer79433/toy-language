#ifndef FUCTIONS_H
#define FUCTIONS_H 1

#include "ast.h"
#include "interp.h"

typedef void (*predefined_func_addr)(toy_interp *interp, toy_expr *result, toy_list *args);
typedef struct predefined_function_struct {
    toy_str name;
    int num_params;
    predefined_func_addr func;
    const toy_char **param_names;
} predefined_function;

const predefined_function *lookup_predefined_function(const toy_str name);

#define INFINITE_PARAMS -1

#endif /* FUCTIONS_H */
