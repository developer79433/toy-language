#ifndef TOY_LEXICAL_STACK_TYPES_H
#define TOY_LEXICAL_STACK_TYPES_H 1

#include <stddef.h>

#include "function-types.h"
#include "buf-stack-types.h"
#include "var-decl-types.h"
#include "stmt-types.h"

typedef struct lexical_stack_entry_struct {
    toy_function *function;
    toy_block *block;
} lexical_stack_entry;

struct lexical_stack_struct {
    struct lexical_stack_struct *next;
    lexical_stack_entry entry;
};
typedef struct lexical_stack_struct lexical_stack;

#endif /* TOY_LEXICAL_STACK_TYPES_H */
