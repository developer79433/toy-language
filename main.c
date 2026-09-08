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
#include "function.h"

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

    toy_parser parser;
    parser_init(&parser);
    parser_assert_valid(&parser);
    toy_function *program = parser_parse(&parser, in);

    add_block_parents(program);
    func_dump(program, TOY_TRUE);

    resolve_names(program);
    allocate_registers(program);

    toy_interp *interp = interp_alloc();
    func_closure closure = { .func = program, .num_closures = 0, .closures = NULL };
    interp_call_func(interp, &closure, NULL);
    interp_free(interp);

    return EXIT_SUCCESS;
}
