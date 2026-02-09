#include <assert.h>

#include "func-closure.h"
#include "function.h"
#include "var.h"
#include "log.h"

static void assert_closures_valid(toy_var *vars, size_t count)
{
    for (toy_var *var = &vars[0]; var < &vars[count]; var++) {
        var_assert_valid(var);
    }
}

void func_closure_assert_valid(const func_closure *closure)
{
    func_assert_valid(closure->func);
    assert(closure->num_closures >= 0);
    assert_closures_valid(closure->closures, closure->num_closures);
}

static void dump_closures(const toy_var *vars, size_t count)
{
    for (const toy_var *var = &vars[0]; var < &vars[count]; var++) {
        var_dump(var, TOY_FALSE);
    }
}

void func_closure_dump(const func_closure *closure, toy_bool verbose)
{
    log_debug("closure { ");
    func_dump(closure->func, verbose);
    dump_closures(closure->closures, closure->num_closures);
    log_debug(" }");
}

static void free_closures(toy_var *vars, size_t count)
{
    /* TODO */
}

void func_closure_free(func_closure *closure)
{
    free_closures(closure->closures, closure->num_closures);
    func_free(closure->func);
}
