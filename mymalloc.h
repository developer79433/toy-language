#ifndef MY_MALLOC_H
#define MY_MALLOC_H 1

#include <stddef.h>
#include <malloc.h>

#define mymalloc(the_type) ((the_type *) malloc(sizeof(the_type)))
void *memcpy_2(void *dst, const void *src1, size_t src1_size, const void *src2, size_t src2_size);
void *malloc_init_2(const void *src1, size_t src1_size, const void *src2, size_t src2_size);

#endif /* MY_MALLOC_H */
