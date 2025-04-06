#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "toy-str.h"

void print_str(FILE *f, const toy_str str)
{
    for (const char *p = str; *p; p++) {
        if (*p == '\'' || *p == '"' || *p == '\\') {
            fputc('\\', f);
        }
        fputc(*p, f);
    }
}

void dump_str(FILE *f, const toy_str str)
{
    fputc('"', f);
    print_str(f, str);
    fputc('"', f);
}

toy_bool toy_str_equal(const toy_str s1, const toy_str s2)
{
    return (0 == strcmp(s1, s2));
}

toy_bool toy_str_nequal(const toy_str s1, const toy_str s2)
{
    return !toy_str_equal(s1, s2);
}

toy_bool toy_str_equal_nocase(const toy_str s1, const toy_str s2)
{
    return (0 == strcasecmp(s1, s2));
}

toy_bool toy_str_nequal_nocase(const toy_str s1, const toy_str s2)
{
    return !toy_str_equal_nocase(s1, s2);
}

#define ONE_MILLION 1000 * 1000

void str_assert_valid(toy_str str)
{
    assert(str);
    assert(strlen(str) < ONE_MILLION);
}
