#ifndef TOY_INTERP_FRAME_TYPES_H
#define TOY_INTERP_FRAME_TYPES_H 1

#include "stmt-list-types.h"
#include "map-val-types.h"

enum frame_type_enum {
    FRAME_LOOP_BODY,
    FRAME_IF_BODY,
    FRAME_PRE_DEF_FUNC,
    FRAME_USER_DEF_FUNC,
    FRAME_BLOCK_STMT
};
typedef enum frame_type_enum frame_type;

typedef struct interp_frame_struct {
    struct interp_frame_struct *prev;
    frame_type type;
    union {
        const toy_block *loop_body;
        const toy_block *if_body;
        const toy_function *pre_def_func;
        const toy_function *user_def_func;
        const toy_block *block_stmt;
    };
    toy_stmt_list *cur_stmt;
    map_val *symbols;
} interp_frame;

#endif /* TOY_INTERP_FRAME_TYPES_H */
