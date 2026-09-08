#include <assert.h>

#include "register-allocator.h"
#include "function.h"
#include "ast-visitor.h"
#include "str-list.h"
#include "str.h"
#include "log.h"
#include "block.h"
#include "errors.h"
#include "list-visitor.h"
#include "decl-ref.h"
#include "decl-ref-list.h"


typedef struct register_allocator_struct {
    ast_visitor ast_vis;
} register_allocator;

static item_callback_result handle_identifier(register_allocator *allocator, toy_identifier *identifier)
{
    item_callback_result res = default_identifier(&allocator->ast_vis, identifier);
    decl_ref *ref = identifier->decl;
    switch (ref->type) {
    case DECL_REF_FUNC:
        /* Function declarations are immutable, so no need for a closure */
        break;
    case DECL_REF_PARAM:
        /* Closure over a function parameter in an enclosing block */
        func_param_ref *param_ref = &ref->func_param;
        log_debug_file("Closure over parameter #%zu to function %s, %zu frames up\n", param_ref->param_index, param_ref->func->name, ref->frames_up);
        break;
    case DECL_REF_PREDEF:
        /* Predefined constants and functions are immutable, so no need for a closure */
        break;
    case DECL_REF_VAR:
        /* Closure over a variable in an enclosing block */
        toy_var_decl *var_decl = ref->var_decl;
        log_debug_file("Closure over variable %s, %zu frames up\n", var_decl->name, ref->frames_up);
        break;
    default:
        assert(0);
        break;
    }
    return res;
}

static register_allocator the_register_allocator = {
    .ast_vis.identifier = (visit_identifier_func) handle_identifier
};

void allocate_registers(toy_function *func)
{
    register_allocator allocator = the_register_allocator;
    item_callback_result res = visit_func_expr(&allocator.ast_vis, func);
    assert(CONTINUE_ENUMERATION == res);
}
