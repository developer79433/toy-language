#ifndef TOY_BLOCK_TYPES_H
#define TOY_BLOCK_TYPES_H 1

#include <stddef.h>

#include "symbol-table.h"

struct toy_stmt_list_struct;
typedef struct toy_stmt_list_struct toy_stmt_list;

struct toy_block_struct;
typedef struct toy_block_struct toy_block;
struct toy_block_struct {
    toy_stmt_list *stmts;
    toy_block *parent;
    symbol_table declaration_symbols;
    symbol_table parameter_symbols;
};

#endif /* TOY_BLOCK_TYPES_H */
