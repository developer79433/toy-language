#ifndef TOY_FUNCTION_TYPES_H
#define TOY_FUNCTION_TYPES_H 1

#include <stddef.h>

#include "block-types.h"
#include "str-types.h"
#include "val-types.h"
#include "val-list-types.h"
#include "str-list-types.h"
#include "interp-types.h"
#include "predef-function-types.h"
#include "symbol-table-types.h"

struct toy_interp_struct;
typedef struct toy_interp_struct toy_interp;

struct toy_stmt_list_struct;
typedef struct toy_stmt_list_struct toy_stmt_list;

enum toy_func_type {
    FUNC_PREDEFINED = 0,
    FUNC_USER_DECLARED = 1
};

struct toy_function_struct;
typedef struct toy_function_struct toy_function;
struct toy_function_struct {
    toy_str name;
    toy_str_list *param_names;
    toy_str doc;
    enum toy_func_type type;
    union {
        toy_block code;
        predefined_func_addr predef;
    };
};

#endif /* TOY_FUNCTION_TYPES_H */
