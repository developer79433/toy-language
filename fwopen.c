#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>

#include "mymalloc.h"
#include "fwopen.h"

FILE *fwopen(const wchar_t *filename, const char *mode) {
    size_t len = wcstombs(NULL, filename, 0);
    if (len == (size_t) -1) {
        perror("wcstombs");
        return NULL;
    }
    char *filename_mbs = mymalloc_array(char, len + 1);
    if (!filename_mbs) {
        return NULL;
    }
    int wcstombs_ret = wcstombs(filename_mbs, filename, len + 1);
    assert(wcstombs_ret >= 0);
    FILE *file = fopen(filename_mbs, mode);
    free(filename_mbs);
    return file;
}
