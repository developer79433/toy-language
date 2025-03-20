#ifndef TOY_INTERP_H
#define TOY_INTERP_H 1

#include <stdio.h>

#include "ast.h"

void dump_expr(FILE *f, const toy_expr *expr);
void eval_expr(toy_expr *result, const toy_expr *expr);
void single_step(const toy_stmt *stmt);
void toy_run(const toy_stmt *stmt);

#endif /* TOY_INTERP_H */
