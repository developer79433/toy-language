#include <assert.h>
#include "test-set-str.h"
#include "set-str.h"
#include "log.h"

void test_set(set_str *set)
{
    assert(!set_str_contains(set, L"foo"));
    assert(!set_str_contains(set, L"bar"));
    assert(!set_str_contains(set, L"baz"));
    /* set_str_dump(set); */

    set_str_add(set, L"foo");
    assert(set_str_contains(set, L"foo"));
    assert(!set_str_contains(set, L"bar"));
    assert(!set_str_contains(set, L"baz"));
    /* set_str_dump(set); */

    set_str_add(set, L"bar");
    assert(set_str_contains(set, L"foo"));
    assert(set_str_contains(set, L"bar"));
    assert(!set_str_contains(set, L"baz"));
    /* set_str_dump(set); */

    set_str_remove(set, L"bar");
    assert(set_str_contains(set, L"foo"));
    assert(!set_str_contains(set, L"bar"));
    assert(!set_str_contains(set, L"baz"));
    /* set_str_dump(set); */

    set_str_add(set, L"baz");
    assert(set_str_contains(set, L"foo"));
    assert(!set_str_contains(set, L"bar"));
    assert(set_str_contains(set, L"baz"));
    /* set_str_dump(set); */

    set_str_add(set, L"bar");
    assert(set_str_contains(set, L"foo"));
    assert(set_str_contains(set, L"bar"));
    assert(set_str_contains(set, L"baz"));
    /* set_str_dump(set); */

    set_str_remove(set, L"foo");
    assert(!set_str_contains(set, L"foo"));
    assert(set_str_contains(set, L"bar"));
    assert(set_str_contains(set, L"baz"));
    /* set_str_dump(set); */
}

void test_set_str(void)
{
    set_str *set1 = set_str_alloc();
    test_set(set1);
    set_str_free(set1);
    set_str set2;
    set_str_init(&set2);
    test_set(&set2);
}
