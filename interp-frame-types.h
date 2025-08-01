#ifndef TOY_INTERP_FRAME_TYPES_H
#define TOY_INTERP_FRAME_TYPES_H 1

#include "stmt-list-types.h"
#include "map-val-types.h"
#include "val-list-types.h"

enum frame_type_enum {
    FRAME_LOOP_BODY,
    FRAME_IF_BODY,
    FRAME_PRE_DEF_FUNC,
    FRAME_USER_DEF_FUNC,
    FRAME_BLOCK_STMT
};
typedef enum frame_type_enum frame_type;

typedef struct function_invocation_struct {
    const toy_function *func;
    const toy_val_list *args;
} function_invocation;

typedef struct interp_frame_struct {
    frame_type type;
    union {
        const toy_block *loop_body;
        const toy_block *if_body;
        /* TODO: These can be merged */
        const function_invocation pre_def_func;
        const function_invocation user_def_func;
        const toy_block *block_stmt;
    };
    toy_stmt_list *cur_stmt;
} interp_frame;

#endif /* TOY_INTERP_FRAME_TYPES_H */
