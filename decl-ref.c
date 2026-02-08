#include <assert.h>

#include "decl-ref.h"
#include "stmt.h"
#include "str.h"
#include "str-list.h"
#include "var-decl.h"
#include "log.h"
#include "function.h"
#include "val.h"

static const char *type_names[DECL_REF_MAX - DECL_REF_MIN + 1] = {
    "function",
    "parameter",
    "predefined",
    "variable"
};

const char *decl_ref_type_name(decl_ref_type type)
{
    return type_names[type - DECL_REF_MIN];
}

void decl_ref_dump(const decl_ref *ref)
{
    log_debug("decl_ref { ");
    switch (ref->type) {
    case DECL_REF_FUNC:
        toy_func_decl_stmt *func_decl_stmt = ref->func_decl;
        func_decl_stmt_dump(func_decl_stmt);
        break;
    case DECL_REF_PARAM:
        const func_param_ref *param_ref = &ref->func_param;
        const toy_function *func = param_ref->func;
        toy_str param_name = str_list_index(func->param_names, param_ref->param_index);
        log_debug("Function parameter #%zd '%s' to function ", param_ref->param_index, param_name);
        func_dump(func, TOY_FALSE);
        break;
    case DECL_REF_PREDEF:
        const toy_val *predef = ref->predef;
        log_debug("Predefined %s", (VAL_FUNC == predef->type) ? "" : "const ");
        val_dump(predef, TOY_FALSE);
        break;
    case DECL_REF_VAR:
        toy_var_decl *var_decl = ref->var_decl;
        var_decl_dump(var_decl);
        break;
    default:
        assert(0);
        break;
    }
    log_debug(" }\n");
}

void decl_ref_assert_valid(const decl_ref *ref)
{
    switch (ref->type) {
    case DECL_REF_FUNC:
        toy_func_decl_stmt *func_decl_stmt = ref->func_decl;
        func_decl_stmt_assert_valid(func_decl_stmt);
        break;
    case DECL_REF_PARAM:
        const func_param_ref *param_ref = &ref->func_param;
        const toy_function *func = param_ref->func;
        func_assert_valid(func);
        assert(param_ref->param_index >= 0);
        assert(param_ref->param_index < list_len((generic_list *) func->param_names));
        break;
    case DECL_REF_PREDEF:
        const toy_val *predef = ref->predef;
        val_assert_valid(predef);
        break;
    case DECL_REF_VAR:
        toy_var_decl *var_decl = ref->var_decl;
        var_decl_assert_valid(var_decl);
        break;
    default:
        assert(0);
        break;
    }
}

toy_bool decl_ref_name_matches(const decl_ref *ref, toy_str desired_name)
{
    switch (ref->type) {
    case DECL_REF_FUNC:
        toy_func_decl_stmt *func_decl_stmt = ref->func_decl;
        toy_str func_name = func_decl_stmt->func->name;
        return str_equal(func_name, desired_name);
    case DECL_REF_PARAM:
        const func_param_ref *param_ref = &ref->func_param;
        const toy_function *func = param_ref->func;
        return str_equal(func->name, desired_name);
    case DECL_REF_PREDEF:
        // const toy_val *predef = ref->predef;
        assert(0);
        break;
    case DECL_REF_VAR:
        toy_var_decl *var_decl = ref->var_decl;
        toy_str var_name = var_decl->name;
        return str_equal(var_name, desired_name);
    default:
        assert(0);
        break;
    }
    assert(0);
    return TOY_FALSE;
}
