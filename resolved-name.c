#include <assert.h>

#include "resolved-name.h"
#include "constants.h"
#include "val.h"
#include "stmt.h"

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

void resolved_name_dump(FILE *f, const resolved_name *resolved)
{
    switch (resolved->type) {
    case REF_FUNC_DECL:
        func_decl_stmt_dump(f, resolved->func_decl);
        break;
    case REF_FUNC_PARAM:
        const func_param_ref *param_ref = &resolved->func_param;
        fprintf(f, "Function parameter #%zd, %zd frames up", param_ref->param_index, param_ref->frames_up);
        break;
    case REF_PREDEF_CONST:
        predef_const_dump(f, resolved->predef_const);
        break;
    case REF_PREDEF_FUNC:
        val_dump(f, resolved->predef_func);
        break;
    case REF_UNDEFINED:
        fputs("Undefined reference", f);
        break;
    case REF_VAR_DECL:
        fprintf(f, "Variable declaration #%zd, %zd frames up", resolved->var_decl.var_index, resolved->var_decl.frames_up);
        break;
    default:
        assert(0);
        break;
    }
}
