#ifndef TOY_PARSER_TYPES_H
#define TOY_PARSER_TYPES_H 1

#include "stmt-list-types.h"
#include "block-types.h"

struct toy_parser_struct;
typedef struct toy_parser_struct toy_parser;
struct toy_parser_struct {
    toy_stmt_list *program_start;
    toy_block *cur_block;
};

#endif /* TOY_PARSER_TYPES_H */
