#ifndef TOY_LEXICAL_FRAME_TYPES_H
#define TOY_LEXICAL_FRAME_TYPES_H 1

#include "block-types.h"
#include "function-types.h"
#include "symbol-table-types.h"

typedef enum lexical_frame_type_enum {
    LEXICAL_FRAME_FUNCTION = 0,
    LEXICAL_FRAME_BLOCK = 1
} lexical_stack_entry_type;

typedef struct lexical_frame_function_struct {
    toy_function *function;
    symbol_table arguments;
} lexical_frame_function;

typedef struct lexical_frame_block_struct {
    toy_block *block;
} lexical_frame_block;

typedef struct lexical_frame_struct {
    lexical_stack_entry_type type;
    union {
        lexical_frame_function function_frame;
        lexical_frame_block block_frame;
    };
    symbol_table variables;
} lexical_frame;

#endif /* TOY_LEXICAL_FRAME_TYPES_H */
