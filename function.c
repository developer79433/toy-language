#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"
#include "function.h"
#include "errors.h"
#include "str-list.h"
#include "symbol-table.h"
#include "val.h"
#include "debug.h"
#include "stmt-list.h"
#include "log.h"
#include "block.h"

void func_dump(const toy_function *func, toy_bool verbose)
{
    func_assert_valid(func);
    log_debug("fun %s(", func->name);
    if (func->param_names == &INFINITE_PARAMS) {
        log_debug("*");
    } else {
        str_list_dump(func->param_names, TOY_FALSE);
    }
    log_debug(")");
    if (verbose) {
        log_debug(" {\n");
        if (func->type == FUNC_PREDEFINED) {
            log_debug("/* Pre-defined function code at %p */\n", func->predef);
        } else if (func->type == FUNC_USER_DECLARED) {
            stmt_list_dump(func->code->stmts);
        } else {
            invalid_function_type(func->type);
        }
        log_debug("}\n");
    }
}

#ifndef NDEBUG
void func_assert_valid(const toy_function *func)
{
    assert(func);
    str_assert_valid(func->name);
    if (&INFINITE_PARAMS == func->param_names) {
        /* No parameter names to validate */
    } else if (func->param_names) {
        str_list_assert_valid(func->param_names);
    }
    if (func->type == FUNC_USER_DECLARED) {
        toy_block *block = func->code;
        block_assert_valid(block);
    }
}
#endif /* NDEBUG */

void func_free(toy_function *func)
{
    func_assert_valid(func);
    free(func);
}
