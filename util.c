#include <stddef.h>

#include "util.h"

void *ptr_array_get_not_null(void **arr, size_t len)
{
    for (void **p = &arr[0]; p < &arr[len]; p++) {
        if (*p) {
            return *p;
        }
    }
    return NULL;
}
