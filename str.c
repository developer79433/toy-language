#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "str.h"
#include "log.h"

void print_str(const toy_str str)
{
    for (const char *p = str; *p; p++) {
        if (*p == '\'' || *p == '"' || *p == '\\') {
            log_putc('\\');
        }
        log_putc(*p);
    }
}

void str_dump(const toy_str str)
{
    log_putc('"');
    print_str(str);
    log_putc('"');
}

toy_bool str_equal(const toy_str s1, const toy_str s2)
{
    return (0 == strcmp(s1, s2));
}

toy_bool str_nequal(const toy_str s1, const toy_str s2)
{
    return !str_equal(s1, s2);
}

toy_bool str_equal_nocase(const toy_str s1, const toy_str s2)
{
    return (0 == strcasecmp(s1, s2));
}

toy_bool str_nequal_nocase(const toy_str s1, const toy_str s2)
{
    return !str_equal_nocase(s1, s2);
}

#define ONE_MILLION 1000 * 1000

void str_assert_valid(toy_str str)
{
    assert(str);
    assert(strlen(str) < ONE_MILLION);
}

toy_str str_concat_alloc(toy_str str1, toy_str str2)
{
    toy_str ret = (toy_str) malloc(strlen(str1) + strlen(str2) + 1);
    sprintf(ret, "%s%s", str1, str2);
    return ret;
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
