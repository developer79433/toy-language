#ifndef TOY_PARSER_H
#define TOY_PARSER_H 1

#include <stdio.h>

#include "toy-parser-types.h"
#include "stmt-list-types.h"

extern toy_parser *the_parser_ptr;

void parser_assert_valid(const toy_parser *parser);
void parser_init(toy_parser *parser);
void parser_parse(toy_parser *parser, FILE *f);
toy_stmt_list *parser_get_program_start(toy_parser *parser);
void parser_set_program_start(toy_parser *parser, toy_stmt_list *stmt_list);
toy_parser *parser_global_get(void);
void parser_global_set(toy_parser *new_parser);
void yyerror(const char *s);

#endif /* TOY_PARSER_H */
