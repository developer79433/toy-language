#include <assert.h>

#include "str.h"
#include "interp.h"
#include "interp-symbol.h"
#include "var-decl-list.h"

typedef struct var_decl_cb_args_struct {
    toy_interp *interp;
} var_decl_cb_args;

static item_callback_result var_decl_callback(void *cookie, size_t index, const toy_var_decl_list *var_decl_list)
{
    var_decl_cb_args *args = (var_decl_cb_args *) cookie;
    const toy_var_decl *decl = var_decl_list_payload_const(var_decl_list);
    str_assert_valid(decl->name);
    expr_assert_valid(decl->value);
    create_variable_expr(args->interp, decl->name, decl->value);
    return CONTINUE_ENUMERATION;
}

run_stmt_result var_decl_stmt(toy_interp *interp, const toy_var_decl_list *var_decl_list)
{
    var_decl_cb_args var_decl_args = { .interp = interp };
    enumeration_result res = var_decl_list_foreach_const(var_decl_list, var_decl_callback, &var_decl_args);
    assert(res == ENUMERATION_COMPLETE);
    return EXECUTED_STATEMENT;
}
