#ifndef TOY_UTIL_H
#define TOY_UTIL_H 1

#include <stddef.h>

#define ELEMENTSOF(arr) (sizeof(arr) / sizeof((arr)[0]))

void *ptr_array_get_not_null(void **arr, size_t len);

#endif /* TOY_UTIL_H */
