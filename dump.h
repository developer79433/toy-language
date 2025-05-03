#ifndef TOY_DUMP_H
#define TOY_DUMP_H

#include <stdio.h>

#include "str-types.h"
#include "expr-types.h"
#include "stmt-types.h"
#include "str-list-types.h"

void identifier_dump(FILE *f, const toy_str str);
void identifier_list_dump(FILE *f, const toy_str_list *list);
void expr_dump(FILE *f, const toy_expr *expr);
void stmt_list_dump(FILE *f, const toy_stmt_list *stmts);
void stmt_dump(FILE *f, const toy_stmt *stmt, int append_semicolon);

#endif /* TOY_DUMP_H */
