#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

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

static toy_function toplevel_function = {
    .code = {
        .stmts = NULL,
        .parent = NULL
    },
    .doc = "Global top-level function",
    .name = "global",
    .param_names = NULL,
    .type = FUNC_USER_DECLARED
};

void parser_assert_valid(const toy_parser *parser)
{
    stmt_list_assert_valid(parser->toplevel_function.code.stmts);
}

void parser_init(toy_parser *parser)
{
    memcpy(&parser->toplevel_function, &toplevel_function, sizeof(parser->toplevel_function));
#if YYDEBUG
#define YYERROR_VERBOSE
    yydebug = 1;
#endif /* YYDEBUG */
}

void yyerror(const char *s)
{  
    log_printf("\nError: %s\n", s);  
}

toy_stmt_list *program_start;

toy_function *parser_parse(toy_parser *parser, FILE *in)
{
    program_start = NULL;
    init_lexer(in);
    int parse_res = yyparse();
    if (0 == parse_res) {
        parser->toplevel_function.code.stmts = program_start;
    } else {
        fprintf(stderr, "yyparse() returned %d\n", parse_res);
        parser->toplevel_function.code.stmts = NULL;
    }
    program_start = NULL;
    return &parser->toplevel_function;
}
