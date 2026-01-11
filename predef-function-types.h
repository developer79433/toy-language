#ifndef TOY_PREDEF_FUNC_TYPES_H
#define TOY_PREDEF_FUNC_TYPES_H 1

#include <stddef.h>

#include "var-types.h"
#include "interp-types.h"

typedef run_stmt_result (*predefined_func_addr)(toy_interp *interp, const toy_var *args, size_t num_args);

#endif /* TOY_PREDEF_FUNC_TYPES_H */
