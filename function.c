#include <assert.h>
#include <string.h>

#include "function.h"
#include "dump.h"
#include "errors.h"

void func_dump(FILE *f, const toy_function *func)
{
    fprintf(f, "fun %s(", func->name);
    if (func->param_names == &INFINITE_PARAMS) {
        fputs("*", f);
    } else {
        identifier_list_dump(f, func->param_names);
    }
    fputs(") {\n", f);
    if (func->type == FUNC_PREDEFINED) {
        fprintf(f, "/* Pre-defined function code at %p */\n", func->predef);
    } else if (func->type == FUNC_USER_DECLARED) {
        stmt_list_dump(f, func->code.stmts);
    } else {
        invalid_function_type(func->type);
    }
    fputs("}\n", f);
}

void func_assert_valid(const toy_function *func)
{
    assert(func);
    /* TODO */
}

void func_free(toy_function *func)
{
    /* TODO */
}
