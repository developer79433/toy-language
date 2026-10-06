#include <assert.h>
#include <string.h>

#include "test-string.h"
#include "str.h"

void test_string(void)
{
    assert(str_equal (L"same", L"same") == TOY_TRUE);
    assert(str_equal (L"not",  L"same") == TOY_FALSE);
    assert(str_nequal(L"same", L"same") == TOY_FALSE);
    assert(str_nequal(L"not",  L"same") == TOY_TRUE);
}
