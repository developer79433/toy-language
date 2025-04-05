#ifndef TOY_FUNCTION_TYPES_H
#define TOY_FUNCTION_TYPES_H 1

#include "toy-str-types.h"
#include "toy-val-types.h"
#include "toy-val-list-types.h"
#include "toy-str-list-types.h"

typedef void (*predefined_func_addr)(toy_val *result, toy_val_list *args);

struct toy_stmt_struct;
typedef struct toy_stmt_struct toy_stmt;

typedef struct toy_block_struct {
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
