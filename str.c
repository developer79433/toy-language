#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "str.h"

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

void str_free(toy_str str)
{
    str_assert_valid(str);
    free(str);
}

static int process_backslash(int in_backslash, const char **src, char **dst)
{
    if (in_backslash) {
        /* in a backslash escape */
        switch (**src) {
        case 'n':
            **dst = '\n';
            (*src)++;
            (*dst)++;
            return 0;
        case 'r':
            **dst = '\r';
            (*src)++;
            (*dst)++;
            return 0;
        case 't':
            **dst = '\t';
            (*src)++;
            (*dst)++;
            return 0;
        case '\0':
            **dst = **src;
            return 0;
        case '\\':
        default:
            **dst = **src;
            (*src)++;
            (*dst)++;
            return 0;
        }
    } else {
        /* not in a backslash escape */
        switch(**src) {
        case '\\':
            (*src)++;
            return 1;
        case '\0':
            **dst = **src;
            return 0;
        default:
            **dst = **src;
            (*src)++;
            (*dst)++;
            return 0;
        }
    }
}

void str_backslash_decode(toy_str dst, const toy_str src, const toy_str src_end)
{
    int in_backslash = 0;
    const char *s;
    char *d;
    for (s = src, d = dst; s <= src_end && *s; ) {
        in_backslash = process_backslash(in_backslash, &s, &d);
    }
}
