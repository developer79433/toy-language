#ifndef TOY_VAL_TYPES_H
#define TOY_VAL_TYPES_H 1

#include <stddef.h>

#include "bool-types.h"
#include "str-types.h"
#include "num-types.h"

struct map_val_struct;
typedef struct map_val_struct map_val;

enum toy_val_type_enum {
    VAL_BOOL = 0,
    VAL_FUNC,
    VAL_LIST,
    VAL_MAP,
    VAL_NULL,
    VAL_NUM,
    VAL_STR
};

#define VAL_MAX VAL_STR

typedef enum toy_val_type_enum toy_val_type;

struct generic_map_struct;
typedef struct generic_map_struct generic_map;

struct toy_val_list_struct;
typedef struct toy_val_list_struct toy_val_list;

struct toy_function_struct;
typedef struct toy_function_struct toy_function;

struct toy_var_struct;
typedef struct toy_var_struct toy_var;

struct func_closure_struct;
typedef struct func_closure_struct func_closure;

typedef struct toy_val_struct {
    toy_val_type type;
    union {
        /* Boolean value */
        toy_bool boolean;
        /* Function value */
        func_closure *closure;
        /* List of values */
        toy_val_list *list;
        /* Object mapping string keys to values */
        map_val *obj;
        /* null */
        /* Floating-point number */
        toy_num num;
        /* Null-terminated string */
        // TODO: Wide character support
        toy_str str;
    };
} toy_val;

#endif /* TOY_VAL_TYPES_H */
