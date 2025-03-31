#ifndef TOY_INTERP_H
#define TOY_INTERP_H 1

#include <stdio.h>

#include "ast.h"

typedef struct toy_interp_struct {
    toy_block top_block;
    toy_map *symbols;
    const toy_block *cur_block; /* TODO: Should be a stack */
} toy_interp;

void init_interp(toy_interp *interp, const toy_stmt *program);
void eval_expr(toy_interp *interp, toy_expr *result, const toy_expr *expr);
void single_step(toy_interp *interp, const toy_stmt *stmt);
void run_block(toy_interp *interp, const toy_block *block);
void step_out(toy_interp *interp);

#endif /* TOY_INTERP_H */
