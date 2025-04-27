#ifndef TOY_DUMP_H
#define TOY_DUMP_H

#include <stdio.h>

#include "toy-str-types.h"
#include "expr-types.h"
#include "stmt-types.h"
#include "toy-str-list-types.h"

void dump_identifier(FILE *f, const toy_str str);
void dump_identifier_list(FILE *f, const toy_str_list *list);
void dump_expr(FILE *f, const toy_expr *expr);
void dump_stmts(FILE *f, const toy_stmt *stmts);
void dump_stmt(FILE *f, const toy_stmt *stmt, int append_semicolon);

#endif /* TOY_DUMP_H */
