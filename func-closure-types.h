#ifndef TOY_FUNC_CLOSURE_TYPES_H
#define TOY_FUNC_CLOSURE_TYPES_H 1

#include <stddef.h>

#include "function-types.h"
#include "var-types.h"

typedef struct func_closure_struct {
    toy_function *func;
    size_t num_closures;
    toy_var *closures;
} func_closure;

#endif /* TOY_FUNC_CLOSURE_TYPES_H */
