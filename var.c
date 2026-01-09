#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "var.h"
#include "mymalloc.h"

toy_var *var_alloc(toy_val *val)
{
    toy_var *var = mymalloc(toy_var);
    if (val) {
        memcpy(&var->val, val, sizeof(var->val));
    }
    var->num_refs = 1;
    return var;
}

toy_var *var_ref(toy_var *var)
{
    var->num_refs++;
    return var;
}

void var_free(toy_var *var)
{
    assert(var->num_refs > 0);
    var->num_refs--;
    if (0 == var->num_refs) {
        free(var);
    }
}
