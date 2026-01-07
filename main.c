#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "stmt.h"
#include "stmt-list.h"
#include "interp.h"
#include "tests.h"
#include "log.h"
#include "name-resolver.h"
#include "toy-parser.h"
#include "add-block-parents.h"
#include "register-allocator.h"

static toy_function toplevel_function = { .code = { .stmts = NULL, .parent = NULL }, .doc = "Global top-level function", .name = "global", .param_names = NULL, .type = FUNC_USER_DECLARED };

static void parse(FILE *in)
{
    toy_parser *parser;
    parser = parser_global_get();
    parser_init(parser);
    parser->cur_block = &toplevel_function.code;
    parser_assert_valid(parser);
    parser_parse(parser, in);
    toy_stmt_list *program_start = parser_get_program_start(parser);
    stmt_list_dump(program_start);
    toplevel_function.code.stmts = program_start;
}

static void name_resolution()
{
    name_resolver resolver;
    resolver_init(&resolver);
    resolve_names(&resolver, &toplevel_function);
}

static void execution()
{
    toy_interp *interp = interp_alloc(toplevel_function.code.stmts);
    interp_run_current_block(interp);
    interp_free(interp);
}

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

    parse(in);
    add_block_parents(&toplevel_function);
    name_resolution();
    allocate_registers(&toplevel_function);
    execution();

    return EXIT_SUCCESS;
}
