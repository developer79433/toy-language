#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include "bool.h"
#include "num-types.h"
#include "str.h"
#include "expr.h"
#include "expr-list.h"
#include "parser.tab.h"
#include "toy-parser.h"
#include "stmt-list.h"
#include "log.h"
#include "block.h"
#include "toy-lexer.h"

void parser_assert_valid(const toy_parser *parser)
{
    stmt_list_assert_valid(parser->program_start);
    block_assert_valid(parser->cur_block);
}

toy_stmt_list *parser_get_program_start(toy_parser *parser)
{
    return parser->program_start;
}

void parser_set_program_start(toy_parser *parser, toy_stmt_list *stmt_list)
{
    parser->program_start = stmt_list;
}

static toy_parser the_parser;
toy_parser *the_parser_ptr = NULL;

toy_parser *parser_global_get(void)
{
    if (NULL == the_parser_ptr) {
        the_parser_ptr = &the_parser;
    }
    return the_parser_ptr;
}

void parser_global_set(toy_parser *new_parser)
{
    the_parser_ptr = new_parser;
}

void parser_init(toy_parser *parser)
{
#if YYDEBUG
#define YYERROR_VERBOSE
    yydebug = 1;
#endif /* YYDEBUG */
}

void yyerror(const char *s)
{  
    log_printf("\nError: %s\n", s);  
}

void parser_parse(toy_parser *parser, FILE *in)
{
    init_lexer(in);
    int parse_res = yyparse();
    if (parse_res != 0) {
        fprintf(stderr, "yyparse() returned %d\n", parse_res);
        exit(EXIT_FAILURE);
    }
}
