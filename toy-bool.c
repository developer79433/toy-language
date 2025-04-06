#include <assert.h>
#include <stdio.h>

#include "toy-bool.h"

void dump_bool(FILE *f, toy_bool b)
{
    fputs(b ? "True" : "False", f);
}

void bool_assert_valid(toy_bool b)
{
    assert(
        (TOY_TRUE  == b) ||
        (TOY_FALSE == b)
    );
}
