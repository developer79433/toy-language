#include <assert.h>

#include "resolved-name.h"
#include "constants.h"
#include "val.h"
#include "stmt.h"
#include "log.h"

toy_bool is_resolved(const resolved_name *resolved)
{
    switch (resolved->type) {
    case REF_UNDEFINED:
        return TOY_FALSE;
    case REF_FUNC_DECL:
        assert(resolved->func_decl);
        return TOY_TRUE;
    case REF_FUNC_PARAM:
        const func_param_ref *param_ref = &resolved->func_param;
        assert(param_ref->frames_up >= 0);
        assert(param_ref->param_index >= 0);
        return TOY_TRUE;
    case REF_VAR_DECL:
        assert(resolved->var_decl.frames_up >= 0);
        assert(resolved->var_decl.var_index >= 0);
        return TOY_TRUE;
    case REF_PREDEF_CONST:
        assert(resolved->predef_const);
        predef_const_assert_valid(resolved->predef_const);
        return TOY_TRUE;
    case REF_PREDEF_FUNC:
        assert(resolved->predef_func);
        val_assert_valid(resolved->predef_func);
        return TOY_TRUE;
    default:
        assert(0);
        break;
    }
}

void resolved_name_dump(const resolved_name *resolved)
{
    switch (resolved->type) {
    case REF_FUNC_DECL:
        func_decl_stmt_dump(resolved->func_decl);
        break;
    case REF_FUNC_PARAM:
        const func_param_ref *param_ref = &resolved->func_param;
        log_printf("Function parameter #%zd, %zd frames up", param_ref->param_index, param_ref->frames_up);
        break;
    case REF_PREDEF_CONST:
        predef_const_dump(resolved->predef_const);
        break;
    case REF_PREDEF_FUNC:
        val_dump(resolved->predef_func);
        break;
    case REF_UNDEFINED:
        log_puts("Undefined reference");
        break;
    case REF_VAR_DECL:
        log_printf("Variable declaration #%zd, %zd frames up", resolved->var_decl.var_index, resolved->var_decl.frames_up);
        break;
    default:
        assert(0);
        break;
    }
}
