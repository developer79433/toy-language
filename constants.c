#include <string.h>

#include "constants.h"
#include "util.h"

const toy_expr null_expr = { .type = EXPR_LITERAL, .val.type = VAL_NULL };
const toy_expr true_expr = { .type = EXPR_LITERAL, .val.type = VAL_BOOL, .val.num = 1 };
const toy_expr false_expr = { .type = EXPR_LITERAL, .val.type = VAL_BOOL, .val.num = 0 };

static predefined_constant predefined_constants[] = {
    { "null", &null_expr },
    { "true", &true_expr },
    { "false", &false_expr }
};

const toy_expr *lookup_predefined_constant(toy_str name)
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
