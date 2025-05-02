#include <stdio.h>
#include <stdlib.h>

#include "dump.h"
#include "expr.h"
#include "stmt.h"
#include "interp.h"
#include "tests.h"
#include "parser.tab.h"

extern toy_stmt_list *get_program_start(void);
extern void init_lexer(FILE *f);
extern void init_parser(void);

int main(int argc, char **argv)
{
    run_tests();

    FILE *in;
    ++argv, --argc;  /* skip over program name */
    if (argc > 0) {
        in = fopen( argv[0], "r" );
    } else {
        in = stdin;
    }

    init_lexer(in);
    init_parser();
    /* yylex(); */
    int parse_res = yyparse();
    if (parse_res != 0) {
        fprintf(stderr, "yyparse() returned %d\n", parse_res);
        return EXIT_FAILURE;
    }
    toy_stmt_list *program_start = get_program_start();
    dump_stmts(stderr, program_start);
    toy_interp *interp = alloc_interp(program_start);
    run_current_block(interp);
    free_interp(interp);
    return EXIT_SUCCESS;
}
