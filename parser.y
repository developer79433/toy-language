%{
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "ast.h"
#include "map.h"
#include "interp.h"

extern int yylex (void);
void yyerror(const char *s);
#define ELEMENTSOF(arr) (sizeof(arr) / sizeof((arr)[0]))
static const char *true_strings[] = {
    "true",
    "on",
    "yes",
    "enable",
    "enabled"
};

int bool2int(const char *boolstr) {
    for (const char **p = &true_strings[0]; p < &true_strings[ELEMENTSOF(true_strings)]; p++) {
        if (0 == strcasecmp(*p, boolstr)) {
            return 1;
        }
    }
    return 0;
}

static toy_stmt *program_start;

%}

%token T_BOOLEAN T_FLOAT T_STRING T_IDENTIFIER T_EQUAL T_COMMA T_COLON T_ASTERISK T_FSLASH T_DOT T_LPAREN T_RPAREN T_LBRACKET T_RBRACKET T_LBRACE T_RBRACE T_SEMICOLON T_PLUS T_MINUS T_IF T_ELSE T_ELSEIF T_FOR T_WHILE T_VAR T_FUN T_AND T_OR T_NOT T_IN T_RETURN

%union {
    toy_bool bool;
    toy_num num;
    toy_str str;
    toy_list *list;
    toy_map *map;
    toy_expr *expr;
    toy_stmt *stmt;
    toy_var_decl *var_decl;
    toy_func_decl_stmt *func_decl;
    toy_if_arm *if_arm;
    toy_block block;
    toy_str_list *str_list;
    toy_map_entry *map_entry;
}

%define parse.error detailed

%type <bool> T_BOOLEAN
%type <num> T_FLOAT
%type <str> T_STRING T_IDENTIFIER
%type <var_decl> vardecl
%type <expr> expr expr_no_comma
%type <str_list> formalparams formalparamlist
%type <stmt> stmts stmt
%type <if_arm> elseifs
%type <var_decl> vardecllist
%type <list> actualargs actualarglist
%type <list> listitems listitemlist
%type <map_entry> mapitem mapitems mapitemlist
%type <block> block elsepart

%left T_COMMA
%right T_ASSIGN
%left T_AND T_OR
%left T_EQUAL T_NEQUAL
%left T_PLUS T_MINUS T_LT T_LTE T_GT T_GTE T_IN
%left T_ASTERISK T_FSLASH T_PERCENT
%left T_NOT T_UNEG

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
    /* TODO: Allow a block instead of a statement */
;

stmt :
    T_IF T_LPAREN expr T_RPAREN block elseifs elsepart {
        $$ = alloc_stmt(STMT_IF);
        $$->if_stmt.arms = alloc_if_arm($3, &$5);
        $$->if_stmt.arms->next = $6;
        $$->if_stmt.elsepart = $7;
    }
    | T_WHILE T_LPAREN expr T_RPAREN block {
        $$ = alloc_stmt(STMT_WHILE);
        $$->while_stmt.condition = $3;
        $$->while_stmt.body = $5;
    }
    | T_FOR T_LPAREN stmt T_SEMICOLON expr T_SEMICOLON stmt T_RPAREN block {
        $$ = alloc_stmt(STMT_FOR);
        $$->for_stmt.at_start = $3;
        $$->for_stmt.condition = $5;
        $$->for_stmt.at_end = $7;
        $$->for_stmt.body = $9;
    }
    | T_SEMICOLON {
        $$ = alloc_stmt(STMT_NULL);
    }
    | expr T_SEMICOLON {
        $$ = alloc_stmt(STMT_EXPR);
        $$->expr_stmt.expr = $1;
    }
    | T_FUN T_IDENTIFIER T_LPAREN formalparams T_RPAREN block {
        $$ = alloc_stmt(STMT_FUNC_DECL);
        $$->func_decl_stmt.def.name = $2;
        $$->func_decl_stmt.def.param_names = $4;
        $$->func_decl_stmt.def.code = $6;
    }
    | T_VAR vardecllist T_SEMICOLON {
        $$ = alloc_stmt(STMT_VAR_DECL);
        $$->var_decl_stmt = $2;
    }
    | T_RETURN T_SEMICOLON {
        $$ = alloc_stmt(STMT_RETURN);
        $$->return_stmt.expr = NULL;
    }
    | T_RETURN expr T_SEMICOLON {
        $$ = alloc_stmt(STMT_RETURN);
        $$->return_stmt.expr = $2;
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
    expr_no_comma T_COLON expr_no_comma {
        $$ = alloc_map_entry($1, $3);
    }
;

expr_no_comma :
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
    | expr_no_comma T_EQUAL expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_EQUAL);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_NEQUAL expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_NEQUAL);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_LT expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_LT);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_LTE expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_LTE);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_GT expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_GT);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_GTE expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_GTE);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_AND expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_AND);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_OR expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_OR);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_IN expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_IN);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | T_NOT expr_no_comma {
        $$ = alloc_unary_op_expr(EXPR_NOT);
        $$->unary_op.arg = $2;
    }
    | expr_no_comma T_PLUS expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_PLUS);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_MINUS expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_MINUS);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_ASTERISK expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_MUL);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | expr_no_comma T_FSLASH expr_no_comma {
        $$ = alloc_binary_op_expr(EXPR_DIV);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
    | T_MINUS expr_no_comma {
        $$ = alloc_unary_op_expr(EXPR_UNEG);
        $$->unary_op.arg = $2;
    }
    | T_LBRACKET listitems T_RBRACKET {
        $$ = alloc_expr(EXPR_LIST);
        $$->list = $2;
    }
    | T_LBRACE mapitems T_RBRACE {
        $$ = alloc_expr(EXPR_MAP);
        $$->map = alloc_map();
        for (toy_map_entry *entry = $2; entry; entry = entry->next) {
            map_set_expr($$->map, entry->key, entry->value);
        }
    }
    | T_FUN T_LPAREN formalparams T_RPAREN block {
        $$ = alloc_expr_func_decl($3, &$5);
    }
    | T_IDENTIFIER T_LPAREN actualargs T_RPAREN {
        $$ = alloc_expr(EXPR_FUNC_CALL);
        $$->func_call.func_name = $1;
        $$->func_call.args = $3;
    }
    | T_IDENTIFIER T_ASSIGN expr_no_comma {
        $$ = alloc_expr(EXPR_ASSIGN);
        $$->assignment.lhs = $1;
        $$->assignment.rhs = $3;
    }
    | T_LPAREN expr T_RPAREN {
        $$ = $2;
    }
;

block :
    T_LBRACE stmts T_RBRACE {
        $$.stmts = $2;
    }
;

expr :
    expr_no_comma {
        $$ = $1;
    }
    | expr T_COMMA expr {
        $$ = alloc_binary_op_expr(EXPR_COMMA);
        $$->binary_op.arg1 = $1;
        $$->binary_op.arg2 = $3;
    }
;

%%

void yyerror(const char *s)
{  
    printf("\nError: %s\n", s);  
}

int main(int argc, char **argv)
{
    extern FILE *yyin;
    ++argv, --argc;  /* skip over program name */
    if (argc > 0) {
        yyin = fopen( argv[0], "r" );
    } else {
        /* yyin = stdin; */
        yyin = fopen( "sample-program.toy", "r" );
    }

    extern void init_lexer(void);
    init_lexer();
    /* yylex(); */
    int parse_res = yyparse();
    if (parse_res != 0) {
        fprintf(stderr, "yyparse() returned %d\n", parse_res);
        return EXIT_FAILURE;
    }
    test_maps();
    dump_stmts(stderr, program_start);
    toy_run(program_start);
    return EXIT_SUCCESS;
}
