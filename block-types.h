#ifndef TOY_BLOCK_TYPES_H
#define TOY_BLOCK_TYPES_H 1

struct toy_stmt_list_struct;
typedef struct toy_stmt_list_struct toy_stmt_list;

typedef struct toy_block_struct {
    toy_stmt_list *stmts;
} toy_block;

#endif /* TOY_BLOCK_TYPES_H */
