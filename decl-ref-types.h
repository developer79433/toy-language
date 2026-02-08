#ifndef TOY_DECL_REF_TYPES_H
#define TOY_DECL_REF_TYPES_H 1

#include <stddef.h>

#include "str-types.h"

typedef enum decl_ref_type_enum {
    DECL_REF_FUNC = 1,
    DECL_REF_PARAM = 2,
    DECL_REF_PREDEF = 3,
    DECL_REF_VAR = 4
} decl_ref_type;

#define DECL_REF_MIN DECL_REF_FUNC
#define DECL_REF_MAX DECL_REF_VAR

struct toy_var_decl_struct;
typedef struct toy_var_decl_struct toy_var_decl;
struct toy_func_decl_stmt_struct;
typedef struct toy_func_decl_stmt_struct toy_func_decl_stmt;
struct toy_function_struct;
typedef struct toy_function_struct toy_function;
struct toy_val_struct;
typedef struct toy_val_struct toy_val;

typedef struct func_param_ref_struct {
    toy_function *func;
    size_t param_index;
} func_param_ref;

typedef struct decl_ref_struct {
    decl_ref_type type;
    union {
        toy_func_decl_stmt *func_decl;
        func_param_ref func_param;
        toy_var_decl *var_decl;
        const toy_val *predef;
    };
} decl_ref;

#endif /* TOY_DECL_REF_TYPES_H */
