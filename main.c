#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "parser.tab.h"
#include "interp.h"
#include "tests.h"

extern toy_stmt *get_program_start(void);
extern void init_lexer(FILE *f);
extern void init_parser(void);

int main(int argc, char **argv)
{
    FILE *in;
    ++argv, --argc;  /* skip over program name */
    if (argc > 0) {
        in = fopen( argv[0], "r" );
    } else {
        /* yyin = stdin; */
        in = fopen( "sample-program.toy", "r" );
    }

    init_lexer(in);
    init_parser();
    /* yylex(); */
    int parse_res = yyparse();
    if (parse_res != 0) {
        fprintf(stderr, "yyparse() returned %d\n", parse_res);
        return EXIT_FAILURE;
    }
    /* run_tests(); */
    toy_stmt *program_start = get_program_start();
    /* dump_stmts(stderr, program_start); */
    toy_interp interp;
    init_interp(&interp, program_start);
    step_out(&interp);
    return EXIT_SUCCESS;
}
