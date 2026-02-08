#ifndef TOY_BLOCK_TYPES_H
#define TOY_BLOCK_TYPES_H 1

#include <stddef.h>

#include "decl-ref-list-types.h"

struct toy_stmt_list_struct;
typedef struct toy_stmt_list_struct toy_stmt_list;

struct toy_block_struct;
typedef struct toy_block_struct toy_block;

struct toy_block_struct {
    toy_stmt_list *stmts;
    toy_block *parent;
    /* FIXME: This is used/consumed by the name resolver. Move it there. */
    decl_ref_list *decls_rev;
    size_t num_closures;
};

#endif /* TOY_BLOCK_TYPES_H */
