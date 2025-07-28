#ifndef TOY_PREDEF_FUNCTION_H
#define TOY_PREDEF_FUNCTION_H 1

#include "function-types.h"
#include "predef-function-types.h"

extern const toy_function predefined_functions[];
extern const toy_val predef_func_vals[];
const toy_function *predef_func_lookup_name(const toy_str name);
const toy_function *predef_func_lookup_addr(predefined_func_addr func_addr);
const toy_val *predef_func_to_val(const toy_function *func);

#endif /* TOY_PREDEF_FUNCTION_H */
