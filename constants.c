#include <string.h>

#include "constants.h"
#include "util.h"

const toy_val null_val = { .type = VAL_NULL };
const toy_expr null_expr = { .type = EXPR_LITERAL, .val = null_val };
const toy_val true_val = { .type = VAL_BOOL, .num = 1 };
const toy_val false_val = { .type = VAL_BOOL, .num = 0 };

static predefined_constant predefined_constants[] = {
    { "null", &null_val },
    { "true", &true_val },
    { "false", &false_val }
};

const toy_val *lookup_predefined_constant(toy_str name)
{
    for (
        const predefined_constant *constant = &predefined_constants[0];
        constant < &predefined_constants[ELEMENTSOF(predefined_constants)];
        constant++)
    {
        if (0 == strcasecmp(constant->name, name)) {
            return constant->value;
        }
    }
    return NULL;
}
