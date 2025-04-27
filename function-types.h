#ifndef TOY_FUNCTION_TYPES_H
#define TOY_FUNCTION_TYPES_H 1

#include "str-types.h"
#include "val-types.h"
#include "val-list-types.h"
#include "str-list-types.h"

struct toy_interp_struct;
typedef struct toy_interp_struct toy_interp;

typedef void (*predefined_func_addr)(toy_interp *interp, toy_val *result, const toy_val_list *args);

struct toy_stmt_struct;
typedef struct toy_stmt_struct toy_stmt;

enum block_type {
    /* TODO: BLOCK_PREDEFINED_FUNC, */
    BLOCK_LOOP_BODY,
    BLOCK_FUNC_BODY,
    BLOCK_IF_BODY,
    BLOCK_BLOCK_STMT_BODY
};

typedef struct toy_block_struct {
    enum block_type type;
    toy_stmt *stmts;
} toy_block;

enum toy_func_type {
    FUNC_PREDEFINED = 0,
    FUNC_USER_DECLARED = 1
};

typedef struct toy_func_def_struct {
    toy_str name;
    toy_str_list *param_names;
    enum toy_func_type type;
    union {
        toy_block code;
        predefined_func_addr predef;
    };
} toy_func_def;

#endif /* TOY_FUNCTION_TYPES_H */
