#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <errno.h>
#include <math.h>

#include "str.h"
#include "log.h"
#include "mymalloc.h"
#include "wcscasestr.h"

void print_str(const toy_str str)
{
    for (const toy_char *p = str; *p; p++) {
        if (*p == L'\'' || *p == L'"' || *p == L'\\') {
            log_putc(LOG_DEBUG, L'\\');
        }
        log_putc(LOG_DEBUG, *p);
    }
}

void str_dump(const toy_str str, toy_bool quoted)
{
    if (quoted) {
        log_putc(LOG_DEBUG, '"');
    }
    print_str(str);
    if (quoted) {
        log_putc(LOG_DEBUG, '"');
    }
}

toy_bool str_equal(const toy_str s1, const toy_str s2)
{
    return (0 == wcscmp(s1, s2));
}

toy_bool str_nequal(const toy_str s1, const toy_str s2)
{
    return !str_equal(s1, s2);
}

toy_bool str_equal_nocase(const toy_str s1, const toy_str s2)
{
    return (0 == wcscasecmp(s1, s2));
}

toy_bool str_nequal_nocase(const toy_str s1, const toy_str s2)
{
    return !str_equal_nocase(s1, s2);
}

toy_str str_find_first(toy_str haystack, toy_str needle)
{
    return wcsstr(haystack, needle);
}

toy_bool str_contains(toy_str haystack, toy_str needle)
{
    toy_str substr = str_find_first(haystack, needle);
    return substr != NULL;
}

toy_str str_find_first_nocase(toy_str haystack, toy_str needle)
{
    return wcscasestr(haystack, needle);
}

toy_bool str_contains_nocase(toy_str haystack, toy_str needle)
{
    toy_str substr = str_find_first_nocase(haystack, needle);
    return substr != NULL;
}

#define ONE_MILLION 1000 * 1000

void str_assert_valid(toy_str str)
{
    assert(str);
    assert(wcslen(str) < ONE_MILLION);
}

toy_str str_concat_alloc(toy_str str1, toy_str str2)
{
    size_t buf_size = wcslen(str1) + wcslen(str2) + 1;
    toy_str buf = mymalloc_array(toy_char, buf_size);
    int swprintf_ret = swprintf(buf, buf_size, L"%ls%ls", str1, str2);
    assert(swprintf_ret == buf_size - 1);
    return buf;
}

void str_free(toy_str str)
{
    str_assert_valid(str);
    free(str);
}

static int mbcslen(const char *mbstr)
{
    return mbstowcs(NULL, mbstr, 0);
}

double str_to_double(const toy_str src)
{
    int saved_errno = errno;
    errno = 0;
    double ret = wcstod(src, NULL);
    if (0 == errno) {
        errno = saved_errno;
    } else {
        ret = NAN;
    }
    return ret;
}

wchar_t *mbcs_to_wcs_alloc(const char *src)
{
    size_t num_chars = mbcslen(src);
    wchar_t *wstr = mymalloc_array(wchar_t, num_chars + 1);
    if (wstr) {
        /* TODO: Use mbrtowc */
        size_t mbstowcs_ret = mbstowcs(wstr, src, num_chars);
        assert(mbstowcs_ret == num_chars);
        assert(wstr[num_chars] == L'\0');
    }
    return wstr;
}

static toy_bool process_backslash(int in_backslash, const char **src, wchar_t **dst, mbstate_t *shift_state)
{
    if (in_backslash) {
        /* in a backslash escape */
        switch (**src) {
        case 'n':
            **dst = L'\n';
            (*src)++;
            (*dst)++;
            return TOY_FALSE;
        case 'r':
            **dst = L'\r';
            (*src)++;
            (*dst)++;
            return TOY_FALSE;
        case 't':
            **dst = L'\t';
            (*src)++;
            (*dst)++;
            return TOY_FALSE;
        case '\0':
            **dst = L'\0';
            (*src)++;
            (*dst)++;
            log_warn(L"Premature end of string after backslash");
            return TOY_FALSE;
        case '\\':
            **dst = L'\\';
            (*src)++;
            (*dst)++;
            return TOY_FALSE;
        default:
            log_warn(L"Unnecessarily backslash-escaped '%c' in string", *src);
            size_t ret = mbsrtowcs(*dst, src, 1, shift_state);
            if (-1 == ret) {
                perror("mbstowcs");
            }
            assert(ret == 1);
            return TOY_FALSE;
        }
    } else {
        /* not in a backslash escape */
        switch(**src) {
        case '\\':
            (*src)++;
            return TOY_TRUE;
        case '\0':
            *dst = L'\0';
            (*src)++;
            (*dst)++;
            return TOY_FALSE;
        default:
            size_t ret = mbsrtowcs(*dst, src, 1, shift_state);
            if (-1 == ret) {
                perror("mbstowcs");
            }
            assert(ret == 1);
            return TOY_FALSE;
        }
    }
}

void str_backslash_decode(toy_str dst, const char *src, const char *src_end)
{
    int in_backslash = 0;
    const char *s;
    wchar_t *d;
    mbstate_t shift_state = { 0 };
    for (s = src, d = dst; s <= src_end && *s; ) {
        in_backslash = process_backslash(in_backslash, &s, &d, &shift_state);
    }
}
