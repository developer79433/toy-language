#ifndef TOY_PREDEF_FUNCTION_H
#define TOY_PREDEF_FUNCTION_H 1

#include "function-types.h"
#include "predef-function-types.h"

const toy_function *func_lookup_predef_name(const toy_str name);
const toy_function *func_lookup_predef_addr(predefined_func_addr func_addr);

#endif /* TOY_PREDEF_FUNCTION_H */
