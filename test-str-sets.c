#include <assert.h>
#include "test-str-sets.h"
#include "set-str.h"
#include "log.h"

void test_set(set_str *set)
{
    assert(!set_str_contains(set, "foo"));
    assert(!set_str_contains(set, "bar"));
    assert(!set_str_contains(set, "baz"));
    /* set_str_dump(set); */

    set_str_add(set, "foo");
    assert(set_str_contains(set, "foo"));
    assert(!set_str_contains(set, "bar"));
    assert(!set_str_contains(set, "baz"));
    /* set_str_dump(set); */

    set_str_add(set, "bar");
    assert(set_str_contains(set, "foo"));
    assert(set_str_contains(set, "bar"));
    assert(!set_str_contains(set, "baz"));
    /* set_str_dump(set); */

    set_str_remove(set, "bar");
    assert(set_str_contains(set, "foo"));
    assert(!set_str_contains(set, "bar"));
    assert(!set_str_contains(set, "baz"));
    /* set_str_dump(set); */

    set_str_add(set, "baz");
    assert(set_str_contains(set, "foo"));
    assert(!set_str_contains(set, "bar"));
    assert(set_str_contains(set, "baz"));
    /* set_str_dump(set); */

    set_str_add(set, "bar");
    assert(set_str_contains(set, "foo"));
    assert(set_str_contains(set, "bar"));
    assert(set_str_contains(set, "baz"));
    /* set_str_dump(set); */

    set_str_remove(set, "foo");
    assert(!set_str_contains(set, "foo"));
    assert(set_str_contains(set, "bar"));
    assert(set_str_contains(set, "baz"));
    /* set_str_dump(set); */
}

void test_str_sets(void)
{
    set_str *set1 = set_str_alloc();
    test_set(set1);
    set_str_free(set1);
    set_str set2;
    set_str_init(&set2);
    test_set(&set2);
}
