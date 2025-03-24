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
%token T_BOOLEAN T_FLOAT T_STRING T_IDENTIFIER T_EQUAL T_COMMA T_COLON T_ASTERISK T_FSLASH T_DOT T_LPAREN T_RPAREN T_LBRACKET T_RBRACKET T_LBRACE T_RBRACE T_SEMICOLON T_PLUS T_MINUS T_IF T_ELSE T_ELSEIF T_FOR T_WHILE T_VAR T_FUN T_AND T_OR T_NOT T_IN
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
    toy_str_list *str_list;
    toy_map_entry *map_entry;
}

%define parse.error detailed

%type <bool> T_BOOLEAN
%type <num> T_FLOAT
%type <str> T_STRING T_IDENTIFIER
%type <var_decl> vardecl
%type <expr> expr
%type <str_list> formalparams formalparamlist
%type <stmt> stmts stmt elsepart
%type <if_arm> elseifs
%type <var_decl> vardecllist
%type <list> actualargs actualarglist
%type <list> listitems listitemlist
%type <map_entry> mapitem mapitems mapitemlist

%right T_ASSIGN
%left T_AND T_OR
%left T_EQUAL T_NEQUAL T_LT T_LTE T_GT T_GTE T_IN
%left T_PLUS T_MINUS
%left T_ASTERISK T_FSLASH T_PERCENT
%left T_NOT T_UNEG

%%

program : stmts stmt {
        program_start = $1;
    }
;

stmts :
    stmts stmt {
        if ($1) {
            append_stmt($1, $2);
            $$ = $1;
        } else {
            $$ = $2;
        }
    }
    | /* EMPTY */ {
        $$ = NULL;
    }
;

stmt :
    T_IF T_LPAREN expr T_RPAREN T_LBRACE stmts T_RBRACE elseifs elsepart {
        $$ = alloc_stmt(STMT_IF);
        $$->if_stmt.arms = alloc_if_arm($3, $6);
        $$->if_stmt.arms->next = $8;
        $$->if_stmt.elsepart = $9;
    }
    | T_WHILE T_LPAREN expr T_RPAREN T_LBRACE stmts T_RBRACE {
        $$ = alloc_stmt(STMT_WHILE);
        $$->while_stmt.condition = $3;
        $$->while_stmt.body = $6;
    }
    | T_FOR T_LPAREN stmt T_SEMICOLON expr T_SEMICOLON stmt T_RPAREN T_LBRACE stmts T_RBRACE {
        $$ = alloc_stmt(STMT_FOR);
        $$->for_stmt.at_start = $3;
        $$->for_stmt.condition = $5;
        $$->for_stmt.at_end = $7;
        $$->for_stmt.body = $10;
    }
    | T_SEMICOLON {
        $$ = alloc_stmt(STMT_NULL);
    }
    | expr T_SEMICOLON {
        $$ = alloc_stmt(STMT_EXPR);
        $$->expr_stmt.expr = $1;
    }
    | T_FUN T_IDENTIFIER T_LPAREN formalparams T_RPAREN T_LBRACE stmts T_RBRACE {
        $$ = alloc_stmt(STMT_FUNC_DECL);
        $$->func_decl_stmt.def.name = $2;
        $$->func_decl_stmt.def.param_names = $4;
        $$->func_decl_stmt.def.code = $7;
    }
    | T_VAR vardecllist T_SEMICOLON {
        $$ = alloc_stmt(STMT_VAR_DECL);
        $$->var_decl_stmt = $2;
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
    | T_IDENTIFIER T_ASSIGN expr {
        $$ = alloc_var_decl($1, $3);
    }
;

elseifs :
    { $$ = NULL; }
    | elseifs T_ELSEIF T_LPAREN expr T_RPAREN T_LBRACE stmts T_RBRACE {
        toy_if_arm *this_arm = alloc_if_arm($4, $7);
        
        if ($1) {
            append_if_arm($1, this_arm);
            $$ = $1;
        } else {
            $$ = this_arm;
        }
    }
;

elsepart :
    { $$ = NULL; }
    | T_ELSE T_LBRACE stmts T_RBRACE {
        $$ = $3;
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

expr :
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
    | expr T_EQUAL expr {
        $$ = alloc_binary_op_expr(EXPR_EQUAL);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_NEQUAL expr {
        $$ = alloc_binary_op_expr(EXPR_NEQUAL);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_LT expr {
        $$ = alloc_binary_op_expr(EXPR_LT);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_LTE expr {
        $$ = alloc_binary_op_expr(EXPR_LTE);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_GT expr {
        $$ = alloc_binary_op_expr(EXPR_GT);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_GTE expr {
        $$ = alloc_binary_op_expr(EXPR_GTE);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_AND expr {
        $$ = alloc_binary_op_expr(EXPR_AND);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_OR expr {
        $$ = alloc_binary_op_expr(EXPR_OR);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_IN expr {
        $$ = alloc_binary_op_expr(EXPR_IN);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | T_NOT expr {
        $$ = alloc_unary_op_expr(EXPR_NOT);
        $$->unary_op->arg = $2;
    }
    | expr T_PLUS expr {
        $$ = alloc_binary_op_expr(EXPR_PLUS);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_MINUS expr {
        $$ = alloc_binary_op_expr(EXPR_MINUS);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_ASTERISK expr {
        $$ = alloc_binary_op_expr(EXPR_MUL);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | expr T_FSLASH expr {
        $$ = alloc_binary_op_expr(EXPR_DIV);
        $$->binary_op->arg1 = $1;
        $$->binary_op->arg2 = $3;
    }
    | T_MINUS expr {
        $$ = alloc_unary_op_expr(EXPR_UNEG);
        $$->unary_op->arg = $2;
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
    | T_FUN T_LPAREN formalparams T_RPAREN T_LBRACE stmts T_RBRACE {
        $$ = alloc_expr_func_decl($3, $6);
    }
    | T_IDENTIFIER T_LPAREN actualargs T_RPAREN {
        $$ = alloc_expr(EXPR_FUNC_CALL);
        $$->func_call->func_name = $1;
        $$->func_call->args = $3;
    }
    | T_LPAREN expr T_RPAREN {
        $$ = $2;
    }
    | T_IDENTIFIER T_ASSIGN expr {
        $$ = alloc_expr(EXPR_ASSIGN);
        $$->assignment->lhs = $1;
        $$->assignment->rhs = $3;
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
    expr {
        $$ = alloc_list($1);
    }
    | actualarglist T_COMMA expr {
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

listitemlist :
    expr {
        $$ = alloc_list($1);
    }
    | listitemlist T_COMMA expr {
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
    expr T_COLON expr {
        $$ = alloc_map_entry($1, $3);
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
    dump_stmts(stderr, program_start);
    toy_run(program_start);
    return EXIT_SUCCESS;
    }
