#ifndef TOY_DUMP_H
#define TOY_DUMP_H

#include <stdio.h>

#include "str-types.h"
#include "expr-types.h"
#include "stmt-types.h"
#include "str-list-types.h"

void dump_identifier(FILE *f, const toy_str str);
void dump_identifier_list(FILE *f, const toy_str_list *list);
void dump_expr(FILE *f, const toy_expr *expr);
void dump_stmt_list(FILE *f, const toy_stmt_list *stmts);
void dump_stmt(FILE *f, const toy_stmt_list *stmt, int append_semicolon);

#endif /* TOY_DUMP_H */
