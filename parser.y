%{
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "ast.h"
#include "map.h"

extern int yylex (void);
void yyerror(const char *s);

static toy_stmt *program_start;

%}

%token T_BOOLEAN T_FLOAT T_STRING T_IDENTIFIER T_EQUAL T_COMMA T_COLON T_ASTERISK T_FSLASH T_DOT T_LPAREN T_RPAREN T_LBRACKET T_RBRACKET T_LBRACE T_RBRACE T_SEMICOLON T_PLUS T_MINUS T_IF T_ELSE T_ELSEIF T_FOR T_WHILE T_VAR T_FUN T_AND T_OR T_NOT T_IN T_RETURN T_BREAK T_CONTINUE T_NULL T_PLUS_PLUS T_MINUS_MINUS T_QUESTION T_ASTERISK_ASTERISK

%union {
    toy_bool bool;
    toy_num num;
    toy_str str;
    toy_list *list;
    toy_map *map;
    toy_expr *expr;
    toy_stmt *stmt;
    toy_var_decl *var_decl;
    toy_if_arm *if_arm;
    toy_block block;
    toy_str_list *str_list;
    toy_map_entry *map_entry;
}

%define parse.error verbose

%type <bool> T_BOOLEAN
%type <num> T_FLOAT
%type <str> T_STRING T_IDENTIFIER
%type <var_decl> vardecl
%type <expr> expr expr_no_comma optional_expr literal prefix_increment prefix_decrement postfix_increment postfix_decrement ternary assignment_expr bracketed_subexpr function_call_expr function_decl_expr map_expr listexpr unary_neg_expr modulus_expr equal_expr nequal_expr lt_expr lte_expr gt_expr gte_expr in_expr and_expr or_expr not_expr add_expr divide_expr multiply_expr subtract_expr comma_expr_with_side_effect comma_expr_without_side_effect expr_with_side_effect_no_comma expr_without_side_effect_no_comma expr_with_side_effect_allow_comma expr_without_side_effect_allow_comma exponent_expr field_ref_expr collection_lookup_expr method_call_expr
%type <str_list> formalparams formalparamlist
%type <stmt> stmts stmt if_stmt while_stmt for_stmt null_stmt expr_stmt func_decl_stmt var_decl_stmt return_stmt break_stmt continue_stmt stmt_requiring_semicolon stmt_in_for_atstart stmt_in_for_atend block_stmt
%type <if_arm> elseifs
%type <var_decl> vardecllist
%type <list> actualargs actualarglist
%type <list> listitems listitemlist
%type <map_entry> mapitem mapitems mapitemlist
%type <block> block elsepart

%left T_COMMA
%right T_ASSIGN T_QUESTION
%left T_AND T_OR
%left T_EQUAL T_NEQUAL
%left T_PLUS T_MINUS
%left T_LT T_LTE T_GT T_GTE
%left T_IN
%left T_ASTERISK T_FSLASH
%left T_PERCENT
%left T_ASTERISK_ASTERISK
%left T_NOT T_UNEG
%left T_DOT

%%

program :
    stmts {
        program_start = $1;
    }
;

stmts :
    /* EMPTY */ {
        $$ = NULL;
    }
    | stmts stmt {
        if ($1) {
            append_stmt($1, $2);
            $$ = $1;
        } else {
            $$ = $2;
        }
    }
;

stmt :
    if_stmt
    | while_stmt
    | for_stmt
    | func_decl_stmt
    | block_stmt
    | stmt_requiring_semicolon T_SEMICOLON
;

stmt_requiring_semicolon:
    null_stmt
    | expr_stmt
    | var_decl_stmt
    | return_stmt
    | break_stmt
    | continue_stmt
;

stmt_in_for_atstart:
    null_stmt
    | expr_stmt
    | var_decl_stmt
;

stmt_in_for_atend:
    null_stmt
    | expr_stmt
;

if_stmt:
    T_IF T_LPAREN expr T_RPAREN block elseifs elsepart {
        $$ = alloc_stmt(STMT_IF);
        $$->if_stmt.arms = alloc_if_arm($3, &$5);
        $$->if_stmt.arms->next = $6;
        $$->if_stmt.elsepart = $7;
    }
;

while_stmt:
    T_WHILE T_LPAREN expr T_RPAREN block {
        $$ = alloc_stmt(STMT_WHILE);
        $$->while_stmt.condition = $3;
        $$->while_stmt.body = $5;
    }
;

for_stmt:
    T_FOR T_LPAREN stmt_in_for_atstart T_SEMICOLON optional_expr T_SEMICOLON stmt_in_for_atend T_RPAREN block {
        $$ = alloc_stmt(STMT_FOR);
        $$->for_stmt.at_start = $3;
        $$->for_stmt.condition = $5;
        $$->for_stmt.at_end = $7;
        $$->for_stmt.body = $9;
    }
;

/**
 * TODO:
 * for (elt in list) block
 * and
 * for (key, value in map) block
 */

null_stmt:
    /* EMPTY */ {
        $$ = alloc_stmt(STMT_NULL);
    }
;

block_stmt:
    block {
        $$ = alloc_stmt(STMT_BLOCK);
        $$->block_stmt.block = $1;
    }
;

expr_stmt:
    expr_with_side_effect_allow_comma {
        $$ = alloc_stmt(STMT_EXPR);
        $$->expr_stmt.expr = $1;
    }
;

func_decl_stmt:
    T_FUN T_IDENTIFIER T_LPAREN formalparams T_RPAREN block {
        $$ = alloc_stmt(STMT_FUNC_DECL);
        $$->func_decl_stmt.def.name = $2;
        $$->func_decl_stmt.def.param_names = $4;
        $$->func_decl_stmt.def.code = $6;
    }
;

var_decl_stmt:
    T_VAR vardecllist {
        $$ = alloc_stmt(STMT_VAR_DECL);
        $$->var_decl_stmt = $2;
    }
;

return_stmt:
    T_RETURN {
        $$ = alloc_stmt(STMT_RETURN);
        $$->return_stmt.expr = NULL;
    }
    | T_RETURN expr {
        $$ = alloc_stmt(STMT_RETURN);
        $$->return_stmt.expr = $2;
    }
;

break_stmt:
    T_BREAK {
        $$ = alloc_stmt(STMT_BREAK);
    }
;

continue_stmt:
    T_CONTINUE {
        $$ = alloc_stmt(STMT_CONTINUE);
    }
;

optional_expr :
    /* EMPTY */ {
        $$ = NULL;
    }
    | expr {
        $$ = $1;
    }
;

vardecllist :
    vardecl {
        $$ = $1;
    }
    | vardecllist T_COMMA vardecl {
        if ($1) {
            append_var_decl($1, $3);
            $$ = $1;
        } else {
            $$ = $3;
        }
    }
;

vardecl :
    T_IDENTIFIER {
        $$ = alloc_var_decl($1, NULL);
    }
    | T_IDENTIFIER T_ASSIGN expr_no_comma {
        $$ = alloc_var_decl($1, $3);
    }
;

elseifs :
    { $$ = NULL; }
    | elseifs T_ELSEIF T_LPAREN expr T_RPAREN block {
        toy_if_arm *this_arm = alloc_if_arm($4, &$6);
        
        if ($1) {
            append_if_arm($1, this_arm);
            $$ = $1;
        } else {
            $$ = this_arm;
        }
    }
    | elseifs T_ELSE T_IF T_LPAREN expr T_RPAREN block {
        toy_if_arm *this_arm = alloc_if_arm($5, &$7);
        
        if ($1) {
            append_if_arm($1, this_arm);
            $$ = $1;
        } else {
            $$ = this_arm;
        }
    }
;

elsepart :
    /* EMPTY */ {}
    | T_ELSE block {
        $$ = $2;
    }
;

formalparams :
    /* EMPTY */ {
        $$ = NULL;
    }
    | formalparamlist {
        $$ = $1;
    }
;

formalparamlist :
    T_IDENTIFIER {
        $$ = alloc_str_list($1);
    }
    | formalparamlist T_COMMA T_IDENTIFIER {
        toy_str_list *this_entry = alloc_str_list($3);
        if ($1) {
            append_str_list($1, this_entry);
            $$ = $1;
        } else {
            $1 = this_entry;
        }
    }
;

actualargs :
    {
        $$ = NULL;
    }
    | actualarglist {
        $$ = $1;
    }
;

actualarglist :
    expr_no_comma {
        $$ = alloc_list($1);
    }
    | actualarglist T_COMMA expr_no_comma {
        toy_list *this_entry = alloc_list($3);
        if ($1) {
            append_list($1, this_entry);
            $$ = $1;
        } else {
            $1 = this_entry;
        }
    }
;

listitems :
    /* EMPTY */ {
        $$ = NULL;
    }
    | listitemlist {
        $$ = $1;
    }
;

listitemlist :
    expr_no_comma {
        $$ = alloc_list($1);
    }
    | listitemlist T_COMMA expr_no_comma {
        toy_list *this_entry = alloc_list($3);
        if ($1) {
            append_list($1, this_entry);
            $$ = $1;
        } else {
            $1 = this_entry;
        }
    }
;

mapitems :
    /* EMPTY */ {
        $$ = NULL;
    }
    | mapitemlist {
        $$ = $1;
    }
;

mapitemlist :
    mapitem
    | mapitemlist T_COMMA mapitem {
        if ($1) {
            append_map_entry($1, $3);
            $$ = $1;
        } else {
            $1 = $3;
        }
    }
;

mapitem :
    T_STRING T_COLON expr_no_comma {
        toy_expr *key = alloc_expr(EXPR_STR);
        key->str = strdup($1);
        $$ = alloc_map_entry(key, $3);
    }
;

literal:
    T_BOOLEAN {
        $$ = alloc_expr(EXPR_BOOL);
        $$->bool = $1;
    }
    | T_FLOAT {
        $$ = alloc_expr(EXPR_NUM);
        $$->num = $1;
    }
    | T_STRING {
        $$ = alloc_expr(EXPR_STR);
        $$->str = $1;
    }
    | T_IDENTIFIER {
        $$ = alloc_expr(EXPR_IDENTIFIER);
        $$->str = $1;
    }
;

/* TODO: Compound assignments: +=, -=, /=, *= */

/* TODO: bit-shift expressions << and >> */
/* TODO: Bitwise operators |, ~, ^ and & */
/* TODO: [] operator */

exponent_expr: expr_no_comma T_ASTERISK_ASTERISK expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_EXPONENT);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
};

/* FIXME: Should allow comma expressions */
assignment_expr: T_IDENTIFIER T_ASSIGN expr_no_comma {
        $$ = alloc_expr(EXPR_ASSIGN);
        $$->assignment.lhs = $1;
        $$->assignment.rhs = $3;
    }
;

field_ref_expr:
    T_IDENTIFIER T_DOT T_IDENTIFIER {
        $$ = alloc_expr(EXPR_FIELD_REF);
        $$->field_ref.lhs = $1;
        $$->field_ref.rhs = $3;
    }
;

/* TODO: This should permit the LHS to be an expression */
collection_lookup_expr:
    T_IDENTIFIER T_LBRACKET expr_no_comma T_RBRACKET {
        $$ = alloc_expr(EXPR_COLLECTION_LOOKUP);
        $$->collection_lookup.lhs = $1;
        $$->collection_lookup.rhs = $3;
    }
;

in_expr:
    expr_no_comma T_IN expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_IN);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

equal_expr:
    expr_no_comma T_EQUAL expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_EQUAL);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

nequal_expr:
    expr_no_comma T_NEQUAL expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_NEQUAL);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

lt_expr:
    expr_no_comma T_LT expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_LT);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

lte_expr:
    expr_no_comma T_LTE expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_LTE);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

gt_expr:
    expr_no_comma T_GT expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_GT);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

gte_expr:
    expr_no_comma T_GTE expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_GTE);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

and_expr:
    expr_no_comma T_AND expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_AND);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

or_expr:
    expr_no_comma T_OR expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_OR);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

not_expr:
    T_NOT expr_no_comma {
        $$ = alloc_unary_op_expr(EXPR_NOT);
        $$->unary_op.arg = $2;
    }
;

add_expr:
    expr_no_comma T_PLUS expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_PLUS);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

subtract_expr:
    expr_no_comma T_MINUS expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_MINUS);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

multiply_expr:
    expr_no_comma T_ASTERISK expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_MUL);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

divide_expr:
    expr_no_comma T_FSLASH expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_DIV);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

modulus_expr: expr_no_comma T_PERCENT expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_MODULUS);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

unary_neg_expr: T_MINUS expr_no_comma {
        $$ = alloc_unary_op_expr(EXPR_UNEG);
        $$->unary_op.arg = $2;
    }
;

listexpr: T_LBRACKET listitems T_RBRACKET {
        $$ = alloc_expr(EXPR_LIST);
        $$->list = $2;
    }
;

map_expr: T_LBRACE mapitems T_RBRACE {
        $$ = alloc_expr(EXPR_MAP);
        $$->map = alloc_map();
        for (toy_map_entry *entry = $2; entry; entry = entry->next) {
            map_set_expr($$->map, entry->key, entry->value);
        }
    }
;

function_decl_expr: T_FUN T_LPAREN formalparams T_RPAREN block {
        $$ = alloc_expr_func_decl($3, &$5);
    }
;

function_call_expr: T_IDENTIFIER T_LPAREN actualargs T_RPAREN {
        $$ = alloc_expr(EXPR_FUNC_CALL);
        $$->func_call.func_name = $1;
        $$->func_call.args = $3;
    }
;

/**
 * TODO: Can this be merged with field_ref?
 * Redefine T_IDENTIFIER to permit a field ref,
 * then method call becomes just another function call.
 */
method_call_expr:
    T_IDENTIFIER T_DOT T_IDENTIFIER T_LPAREN actualargs T_RPAREN {
        $$ = alloc_expr(EXPR_METHOD_CALL);
        $$->method_call.target = $1;
        $$->method_call.func_name = $3;
        $$->method_call.args = $5;
    }
;

bracketed_subexpr: T_LPAREN expr T_RPAREN {
        $$ = $2;
    }
;

postfix_decrement: T_IDENTIFIER T_MINUS_MINUS {
        $$ = alloc_expr(EXPR_POSTFIX_DECREMENT);
        $$->postfix_decrement.id = $1;
    }
;

postfix_increment: T_IDENTIFIER T_PLUS_PLUS {
        $$ = alloc_expr(EXPR_POSTFIX_INCREMENT);
        $$->postfix_increment.id = $1;
    }
;

prefix_decrement: T_MINUS_MINUS T_IDENTIFIER {
        $$ = alloc_expr(EXPR_PREFIX_DECREMENT);
        $$->prefix_decrement.id = $2;
    }
;

prefix_increment: T_PLUS_PLUS T_IDENTIFIER {
        $$ = alloc_expr(EXPR_PREFIX_INCREMENT);
        $$->prefix_increment.id = $2;
    }
;

ternary: expr_no_comma T_QUESTION expr_no_comma T_COLON expr_no_comma %prec T_QUESTION {
        $$ = alloc_expr(EXPR_TERNARY);
        $$->ternary.condition = $1;
        $$->ternary.if_true = $3;
        $$->ternary.if_false = $5;
    }
;

comma_expr_with_side_effect:
    expr_with_side_effect_allow_comma T_COMMA expr_with_side_effect_allow_comma {
        $$ = alloc_binary_op_expr(EXPR_COMMA);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

comma_expr_without_side_effect:
    expr_without_side_effect_allow_comma T_COMMA expr_without_side_effect_allow_comma {
        $$ = alloc_binary_op_expr(EXPR_COMMA);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

expr_with_side_effect_no_comma :
    function_call_expr
    | method_call_expr
    | assignment_expr
    | postfix_decrement
    | postfix_increment
    | prefix_decrement
    | prefix_increment
;

expr_with_side_effect_allow_comma :
    expr_with_side_effect_no_comma
    | comma_expr_with_side_effect
;

expr_without_side_effect_no_comma :
    literal {
        $$ = $1;
    }
    | equal_expr
    | nequal_expr
    | lt_expr
    | lte_expr
    | gt_expr
    | gte_expr
    | and_expr
    | or_expr
    | in_expr
    | not_expr
    | add_expr
    | subtract_expr
    | multiply_expr
    | divide_expr
    | map_expr
    | modulus_expr
    | unary_neg_expr
    | listexpr
    | function_decl_expr
    | bracketed_subexpr
    | ternary
    | exponent_expr
    | field_ref_expr
    | collection_lookup_expr
;

expr_without_side_effect_allow_comma :
    expr_without_side_effect_no_comma
    | comma_expr_without_side_effect
;

/*
for loop: SE required, commas optional
func args: SE optional, commas forbidden
map/list: SE optional, commas forbidden
*/

expr_no_comma :
    expr_with_side_effect_no_comma
    | expr_without_side_effect_no_comma
;

expr :
    expr_with_side_effect_allow_comma
    | expr_without_side_effect_allow_comma
;

block :
    T_LBRACE stmts T_RBRACE {
        $$.stmts = $2;
    }
;

%%

void yyerror(const char *s)
{  
    printf("\nError: %s\n", s);  
}

void init_parser(void)
{
#if YYDEBUG
#define YYERROR_VERBOSE
    yydebug = 1;
#endif /* YYDEBUG */
}

toy_stmt *get_program_start(void)
{
    return program_start;
}
