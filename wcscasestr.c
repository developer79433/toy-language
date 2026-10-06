#include <wchar.h>
#include <wctype.h>

wchar_t *wcscasestr(const wchar_t *haystack, const wchar_t *needle) {
    if (!*needle) {
        return (wchar_t *) haystack;
    }
    
    for (; *haystack; haystack++) {
        if (towlower(*haystack) == towlower(*needle)) {
            const wchar_t *h = haystack;
            const wchar_t *n = needle;
            while (*h && *n && towlower(*h) == towlower(*n)) {
                ++h;
                ++n;
            }
            if (!*n) {
                return (wchar_t *) haystack;
            }
        }
    }
    return NULL;
}
