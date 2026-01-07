#include "toy-lexer.h"

void lexer_init(toy_lexer *lexer, FILE *f)
{
    lexer->input = f;
}
