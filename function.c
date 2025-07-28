#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"
#include "function.h"
#include "dump.h"
#include "errors.h"
#include "str-list.h"
#include "symbol-table.h"
#include "val.h"
#include "debug.h"

void func_dump(FILE *f, const toy_function *func)
{
    func_assert_valid(func);
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

#ifndef NDEBUG
void func_assert_valid(const toy_function *func)
{
    assert(func);
    str_assert_valid(func->name);
    if (func->param_names) {
        if (valid_check_depth < VALID_CHECK_RECURSION_DEPTH) {
            valid_check_depth++;
            str_list_assert_valid(func->param_names);
            valid_check_depth--;
        }
    }
}
#endif /* NDEBUG */

void func_free(toy_function *func)
{
    func_assert_valid(func);
    free(func);
}
