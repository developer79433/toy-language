#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>

#include "ast.h"
#include "map.h"

void print_str(FILE *f, const toy_str str);
void dump_str(FILE *f, const toy_str str);
void dump_identifier(FILE *f, const toy_str str);
void dump_identifier_list(FILE *f, const toy_str_list *list);
void dump_list(FILE *f, toy_list *list);
void dump_expr(FILE *f, const toy_expr *expr);
void dump_stmts(FILE *f, const toy_stmt *stmts);
void dump_stmt(FILE *f, const toy_stmt *stmt, int append_semicolon);

#endif /* DUMP_H */
