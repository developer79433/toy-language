#ifndef TOY_LEXICAL_STACK_TYPES_H
#define TOY_LEXICAL_STACK_TYPES_H 1

#include "lexical-frame-types.h"

struct lexical_stack_struct {
    struct lexical_stack_struct *next;
    lexical_frame entry;
};
typedef struct lexical_stack_struct lexical_stack;

#endif /* TOY_LEXICAL_STACK_TYPES_H */
