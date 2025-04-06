#ifndef TOY_INTER_TYPES_H
#define TOY_INTER_TYPES_H 1

struct toy_interp_struct;
typedef struct toy_interp_struct toy_interp;

enum run_stmt_result {
    EXECUTED_STATEMENT,
    REACHED_RETURN,
    REACHED_BREAK,
    REACHED_CONTINUE,
    REACHED_BLOCK_END
};

#endif /* TOY_INTER_TYPES_H */
