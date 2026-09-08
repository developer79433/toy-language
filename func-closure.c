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
    assert(
        (0 == closure->num_closures && NULL == closure->closures) ||
        (0 != closure->num_closures && NULL != closure->closures)
    );
    assert_closures_valid(closure->closures, closure->num_closures);
}

static void dump_closures(const toy_var *vars, size_t count)
{
    if (count) {
        log_debug(" closed_vars [ ");
        for (const toy_var *var = &vars[0]; var < &vars[count]; var++) {
            var_dump(var, TOY_FALSE);
        }
        log_debug("]");
    }
}

void func_closure_dump(const func_closure *closure, toy_bool verbose)
{
    if (closure->num_closures) {
        log_debug("closure { ");
        func_dump(closure->func, verbose);
        dump_closures(closure->closures, closure->num_closures);
        log_debug(" }");
    } else {
        func_dump(closure->func, verbose);
    }
}

void func_closure_free(func_closure *closure)
{
    for (
        toy_var *var = &closure->closures[0];
        var < &closure->closures[closure->num_closures];
        var++
    ) {
        var_free(var);
    }
    closure->num_closures = 0;
    closure->closures = NULL;
    func_free(closure->func);
}
