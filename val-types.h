#ifndef TOY_VAL_TYPES_H
#define TOY_VAL_TYPES_H 1

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

struct toy_func_def_struct;
typedef struct toy_func_def_struct toy_func_def;

typedef struct toy_val_struct {
    toy_val_type type;
    union {
        toy_bool boolean;
        toy_func_def *func;
        toy_val_list *list;
        map_val *map;
        // null
        toy_num num;
        toy_str str;
    };
} toy_val;

#endif /* TOY_VAL_TYPES_H */
