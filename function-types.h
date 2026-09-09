#ifndef TOY_FUNCTION_TYPES_H
#define TOY_FUNCTION_TYPES_H 1

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

typedef enum toy_func_type_enum {
    FUNC_PREDEFINED = 1,
    FUNC_USER_DECLARED = 2
} toy_func_type;

#define FUNC_MIN FUNC_PREDEFINED
#define FUNC_MAX FUNC_USER_DECLARED

struct toy_function_struct;
typedef struct toy_function_struct toy_function;
struct toy_function_struct {
    toy_str name;
    toy_str_list *param_names;
    size_t num_closures;
    toy_str doc;
    toy_func_type type;
    union {
        toy_block *code;
        predefined_func_addr predef;
    };
};

#endif /* TOY_FUNCTION_TYPES_H */
