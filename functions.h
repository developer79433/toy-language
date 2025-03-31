#ifndef FUCTIONS_H
#define FUCTIONS_H 1

#include "ast.h"
#include "interp.h"

typedef void (*predefined_func_addr)(toy_interp *interp, toy_expr *result, toy_list *args);
predefined_func_addr lookup_predefined_function(const toy_str name);

#endif /* FUCTIONS_H */
