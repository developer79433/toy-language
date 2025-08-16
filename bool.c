#include <assert.h>

#include "bool.h"
#include "log.h"

void dump_bool(toy_bool b)
{
    log_puts(b ? "True" : "False");
}

void bool_assert_valid(toy_bool b)
{
    assert(
        (TOY_TRUE  == b) ||
        (TOY_FALSE == b)
    );
}
