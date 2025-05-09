#include <string.h>

#include "mymalloc.h"

void *malloc_init_2(const void *src1, size_t src1_size, const void *src2, size_t src2_size)
{
    void *ptr = malloc(src1_size + src2_size);
    memcpy_2(ptr, src1, src1_size, src2, src2_size);
    return ptr;
}

void *memcpy_2(void *dst, const void *src1, size_t src1_size, const void *src2, size_t src2_size)
{
    char *p = (char *) dst;
    if (src1) {
        void *ret1 = memcpy(p, src1, src1_size);
        if (src2) {
            p += src1_size;
            return memcpy(p, src2, src2_size);
        }
        return ret1;
    }
    return NULL;
}
