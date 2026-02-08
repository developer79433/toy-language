#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"
#include "constant.h"
#include "util.h"
#include "predef-function.h"
#include "val.h"

const toy_val null_val = { .type = VAL_NULL };
const toy_expr null_expr = { .type = EXPR_LITERAL, .val = (toy_val *) &null_val };
const toy_val true_val = { .type = VAL_BOOL, .num = 1 };
const toy_val false_val = { .type = VAL_BOOL, .num = 0 };

static const predefined_constant predefined_constants[] = {
    { "null", null_val },
    { "true", true_val },
    { "false", false_val }
};

static int compare_constant_names(const void *p1, const void *p2)
{
    const char *name1 = p1, *name2 = p2;
    return strcmp(name1, name2);
}

const predefined_constant *constant_get(toy_str name)
{
    predefined_constant look_for_const = { .name = name };
    return bsearch(&look_for_const, predefined_constants, ELEMENTSOF(predefined_constants), sizeof(predefined_constants[0]), compare_constant_names);
}

const toy_val *constant_get_val(toy_str name)
{
    const predefined_constant *constant = constant_get(name);
    if (constant) {
        return &constant->value;
    }
    return NULL;
}

toy_bool is_constant(toy_str name)
{
    if (constant_get(name)) {
        return TOY_TRUE;
    }
    if (predef_func_lookup_name(name)) {
        return TOY_TRUE;
    }
    return TOY_FALSE;
}

void constant_dump(const predefined_constant *predef_const)
{
    str_dump(predef_const->name, TOY_FALSE);
    val_dump(&predef_const->value, 1);
}

void constant_assert_valid(const predefined_constant *predef_const)
{
    str_assert_valid(predef_const->name);
    val_assert_valid(&predef_const->value);
}
