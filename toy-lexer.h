#ifndef TOY_LEXER_H
#define TOY_LEXER_H 1

#include "toy-lexer-types.h"

void init_lexer(FILE *f);
void lexer_init(toy_lexer *lexer, FILE *f);

#endif /* TOY_LEXER_H */
