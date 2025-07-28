#ifndef TOY_DEBUG_H
#define TOY_DEBUG_H 1

#include <stddef.h>

#ifndef NDEBUG
#define VALID_CHECK_RECURSION_DEPTH 5
#endif /* NDEBUG */

#ifndef NDEBUG
extern size_t valid_check_depth;
#endif /* NDEBUG */

#endif /* TOY_DEBUG_H */
