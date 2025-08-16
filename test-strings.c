#include <assert.h>
#include <string.h>

#include "test-strings.h"
#include "str.h"

void test_strings(void)
{
    assert(str_equal("same", "same") == TOY_TRUE);
    assert(str_equal("not", "same") == TOY_FALSE);
    assert(str_nequal("same", "same") == TOY_FALSE);
    assert(str_nequal("not", "same") == TOY_TRUE);
}
