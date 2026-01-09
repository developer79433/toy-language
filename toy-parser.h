#ifndef TOY_PARSER_H
#define TOY_PARSER_H 1

#include <stdio.h>

#include "toy-parser-types.h"
#include "stmt-list-types.h"

extern toy_parser *the_parser_ptr;

void parser_assert_valid(const toy_parser *parser);
void parser_init(toy_parser *parser);
toy_function *parser_parse(toy_parser *parser, FILE *f);
void yyerror(const char *s);

#endif /* TOY_PARSER_H */
