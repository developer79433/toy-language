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

/* TODO: Do something useful */

typedef struct register_allocator_struct {
    ast_visitor ast_vis;
} register_allocator;

static register_allocator the_register_allocator = {
};

void allocate_registers(toy_function *func)
{
    register_allocator allocator = the_register_allocator;
    item_callback_result res = visit_func_expr((ast_visitor *) &allocator, func);
    assert(CONTINUE_ENUMERATION == res);
}
