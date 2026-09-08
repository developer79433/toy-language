#ifndef TOY_INTERP_FRAME_TYPES_H
#define TOY_INTERP_FRAME_TYPES_H 1

#include "val-types.h"
#include "stmt-list-types.h"
#include "map-val-types.h"
#include "val-list-types.h"
#include "var-types.h"

enum frame_type_enum {
    FRAME_BLOCK_STMT    = 0,
    FRAME_IF_BODY       = 1,
    FRAME_LOOP_BODY     = 2,
    FRAME_PRE_DEF_FUNC  = 3,
    FRAME_USER_DEF_FUNC = 4
};
typedef enum frame_type_enum frame_type;

#define FRAME_MIN FRAME_BLOCK_STMT
#define FRAME_MAX FRAME_USER_DEF_FUNC

typedef struct func_call_frame_struct {
    const func_closure *closure;
    size_t num_arguments;
    toy_var *arguments;
} func_call_frame;

typedef struct block_frame_struct {
    const toy_block *block;
} block_frame;

typedef struct interp_frame_struct {
    frame_type type;
    union {
        const block_frame block_stmt;
        func_call_frame func_call;
    };
    toy_stmt_list *cur_stmt;
    size_t num_variables;
    toy_var *variables;
} interp_frame;

#endif /* TOY_INTERP_FRAME_TYPES_H */
