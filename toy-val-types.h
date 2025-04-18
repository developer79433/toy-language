#ifndef TOY_VAL_TYPES_H
#define TOY_VAL_TYPES_H 1

#include "toy-bool-types.h"
#include "toy-str-types.h"
#include "toy-num-types.h"

enum toy_val_type {
    VAL_BOOL = 0,
    VAL_FUNC,
    VAL_LIST,
    VAL_MAP,
    VAL_NULL,
    VAL_NUM,
    VAL_STR
};

struct toy_map_struct;
typedef struct toy_map_struct toy_map;

struct toy_val_list_struct;
typedef struct toy_val_list_struct toy_val_list;

struct toy_func_def_struct;
typedef struct toy_func_def_struct toy_func_def;

typedef struct toy_val_struct {
    enum toy_val_type type;
    union {
        toy_bool boolean;
        toy_func_def *func;
        toy_val_list *list;
        toy_map *map;
        // null
        toy_num num;
        toy_str str;
    };
} toy_val;

#endif /* TOY_VAL_TYPES_H */
