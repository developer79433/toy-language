%{
#include <stdio.h>
extern int yylex (void);
void yyerror(char *s);
%}
%token T_BOOLEAN T_FLOAT T_STRING T_EQUAL T_COMMA T_COLON T_ASTERISK T_FSLASH T_DOT T_LPAREN T_RPAREN T_LBRACKET T_RBRACKET T_LBRACE T_RBRACE T_SEMICOLON T_PLUS T_MINUS T_IF T_ELSE T_FOR T_WHILE T_VAR T_FUN T_AND T_OR T_NOT T_IDENTIFIER
%left T_PLUS T_MINUS
%left T_ASTERISK T_FSLASH
%left T_NOT T_AND T_OR
%%
program : stmt | stmts stmt ;
stmt : evalstmt | ifstmt | forstmt | whilestmt | vardecls | funcdecl ;
evalstmt : expr T_SEMICOLON ;
ifstmt : T_IF T_LPAREN expr T_RPAREN block | T_IF T_LPAREN expr T_RPAREN block T_ELSE block;
forstmt : T_FOR T_LPAREN vardecls stmt T_SEMICOLON stmt T_RPAREN block
whilestmt : T_WHILE T_LPAREN expr T_RPAREN block
block : T_LBRACE T_RBRACE | T_LBRACE stmts T_RBRACE ;
stmts: stmt | stmts stmt ;
funcdecl : T_FUN T_IDENTIFIER T_LPAREN T_RPAREN block ;
expr : literal | andexpr | orexpr | notexpr | addexpr | subexpr | mulexpr | divexpr | unegexpr | T_LPAREN expr T_RPAREN | listexpr | mapexpr | funcexpr | callexpr ;
literal : T_BOOLEAN | T_FLOAT | T_STRING ;
callexpr : T_IDENTIFIER T_LPAREN actualargs T_RPAREN ;
actualargs : actualarg | actualargs T_COMMA actualarg ;
actualarg : expr ;
listexpr : T_LBRACKET listitems T_RBRACKET ;
listitems : listitem | listitems T_COMMA listitem ;
listitem : expr ;
mapexpr : T_LBRACE mapitems T_RBRACE ;
mapitems : mapitem | mapitems T_COMMA mapitem ;
mapitem : expr T_COLON expr ;
funcexpr : T_FUN T_LPAREN T_RPAREN block ;
addexpr : expr T_PLUS expr ;
subexpr : expr T_MINUS expr ;
mulexpr : expr T_ASTERISK expr ;
divexpr : expr T_FSLASH expr ;
unegexpr : T_MINUS expr ;
andexpr : expr T_AND expr ;
orexpr : expr T_OR expr ;
notexpr : T_NOT expr ;
vardecls : T_VAR varlist T_SEMICOLON ;
varlist : varcomp | varlist T_COMMA varcomp ;
varcomp : T_IDENTIFIER | T_IDENTIFIER T_EQUAL expr ;
%%

void yyerror(char *s)  
{  
 printf("\nError: %s\n", s);  
}

int main(int argc, char **argv)
    {
    extern FILE *yyin;
#ifdef YYDEBUG
    yydebug = 1;
#endif
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
            yyin = fopen( argv[0], "r" );
    else
            yyin = stdin;

    /* yylex(); */
    yyparse();
    return 0;
    }
