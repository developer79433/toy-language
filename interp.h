#ifndef TOY_INTERP_H
#define TOY_INTERP_H 1

#include "ast.h"

void eval_expr(toy_expr *result, const toy_expr *expr);
void exec(toy_expr *result, const toy_stmt *stmt);

#endif /* TOY_INTERP_H */
