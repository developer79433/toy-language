#include <assert.h>
#include <string.h>

#include "test-strings.h"
#include "str.h"

void test_strings(void)
{
    assert(toy_str_equal("same", "same") == TOY_TRUE);
    assert(toy_str_equal("not", "same") == TOY_FALSE);
    assert(toy_str_nequal("same", "same") == TOY_FALSE);
    assert(toy_str_nequal("not", "same") == TOY_TRUE);
}
