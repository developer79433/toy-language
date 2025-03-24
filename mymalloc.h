#ifndef MY_MALLOC_H
#define MY_MALLOC_H 1

#include <malloc.h>

#define mymalloc(the_type) ((the_type *) malloc(sizeof(the_type)))

#endif /* MY_MALLOC_H */
