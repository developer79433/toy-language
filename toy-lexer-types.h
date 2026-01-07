#ifndef TOY_LEXER_TYPES_H
#define TOY_LEXER_TYPES_H 1

#include <stdio.h>

struct toy_lexer_struct;
typedef struct toy_lexer_struct toy_lexer;
struct toy_lexer_struct {
    FILE *input;
};

#endif /* TOY_LEXER_TYPES_H */
