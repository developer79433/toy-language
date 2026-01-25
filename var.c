#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "var.h"
#include "mymalloc.h"
#include "val.h"
#include "log.h"

void var_init(toy_var *var)
{
    var->num_refs = 1;
    var->val.type = VAL_NULL;
}

toy_var *var_alloc_ref(toy_val *val)
{
    val_assert_valid(val);
    toy_var *var = mymalloc(toy_var);
    var_init(var);
    memcpy(&var->val, val, sizeof(var->val));
    var_assert_valid(var);
    return var;
}

toy_var *var_alloc_array(size_t count)
{
    toy_var *vars = mymalloc_array(toy_var, count);
    for (toy_var *var = vars; var < vars + count; var++) {
        var_init(var);
    }
    return vars;
}

toy_var *var_ref(toy_var *var)
{
    var->num_refs++;
    return var;
}

toy_val *var_get(toy_var *var)
{
    return &var->val;
}

const toy_val *var_get_const(const toy_var *var)
{
    return &var->val;
}

void var_set(toy_var *var, const toy_val *new_val)
{
    var_assert_valid(var);
    val_assert_valid(new_val);
    memcpy(&var->val, new_val, sizeof(var->val));
}

void var_free(toy_var *var)
{
    assert(var->num_refs > 0);
    var->num_refs--;
    if (0 == var->num_refs) {
        free(var);
    }
}

void var_assert_valid(const toy_var *var)
{
    assert(var->num_refs >= 0);
    val_assert_valid(&var->val);
}

void var_array_assert_valid(const toy_var *vars, size_t count)
{
    for (const toy_var *var = vars; var < vars + count; var++) {
        var_assert_valid(var);
    }
}

void var_dump(const toy_var *var, toy_bool verbose)
{
    if (verbose) {
        log_printf("var {\n");
        log_printf("num_refs: %d\n", var->num_refs);
        log_printf("val:\n");
        val_dump(&var->val, verbose);
        log_printf("} var\n");
    } else {
        const toy_val *val = var_get_const(var);
        val_dump(val, verbose);
    }
}

void var_array_dump(const toy_var *vars, size_t size, toy_bool verbose)
{
    toy_bool printed_anything = TOY_FALSE;
    for (const toy_var *var = vars; var < &vars[size]; var++) {
        if (printed_anything) {
            log_puts(", ");
        }
        var_dump(var, verbose);
        printed_anything = TOY_TRUE;
    }
}

void var_array_free(toy_var *vars, size_t size)
{
    for (toy_var *var = vars; var < vars + size; var++) {
        var_free(var);
    }
}
