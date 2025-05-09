#ifndef TOY_INTER_TYPES_H
#define TOY_INTER_TYPES_H 1

enum frame_type_enum {
    FRAME_LOOP_BODY,
    FRAME_IF_BODY,
    FRAME_PRE_DEF_FUNC,
    FRAME_USER_DEF_FUNC,
    FRAME_BLOCK_STMT
};
typedef enum frame_type_enum frame_type;

struct toy_interp_struct;
typedef struct toy_interp_struct toy_interp;

typedef enum run_stmt_result_enum {
    EXECUTED_STATEMENT,
    REACHED_RETURN,
    REACHED_BREAK,
    REACHED_CONTINUE,
    /* TODO: Can this be merged with EXECUTED_STATEMENT? */
    REACHED_BLOCK_END
} run_stmt_result;

#endif /* TOY_INTER_TYPES_H */
