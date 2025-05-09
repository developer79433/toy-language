#ifndef TOY_FUNCTION_TYPES_H
#define TOY_FUNCTION_TYPES_H 1

#include "str-types.h"
#include "val-types.h"
#include "val-list-types.h"
#include "str-list-types.h"
#include "interp-types.h"

struct toy_interp_struct;
typedef struct toy_interp_struct toy_interp;

typedef run_stmt_result (*predefined_func_addr)(toy_interp *interp, const toy_val_list *args);

struct toy_stmt_list_struct;
typedef struct toy_stmt_list_struct toy_stmt_list;

typedef struct toy_block_struct {
    toy_stmt_list *stmts;
} toy_block;

enum toy_func_type {
    FUNC_PREDEFINED = 0,
    FUNC_USER_DECLARED = 1
};

/* TODO: Should be named toy_function */
typedef struct toy_func_def_struct {
    toy_str name;
    toy_str_list *param_names;
    toy_str doc;
    enum toy_func_type type;
    union {
        toy_block code;
        predefined_func_addr predef;
    };
} toy_func_def;

#endif /* TOY_FUNCTION_TYPES_H */
