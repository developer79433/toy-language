%{
#include <stdio.h>
extern int yylex (void);
void yyerror(char *s);
%}
%token T_BOOLEAN T_FLOAT T_STRING T_OPERATOR T_EQUAL T_COMMA T_SEMICOLON T_IF T_FOR T_WHILE T_VAR T_FUN T_IDENTIFIER
%%
program : typedecls ;
typedecls : typedecl | typedecls typedecl ;
typedecl : vardecl | funcdecl | evalexpr ;
funcdecl : T_FUN T_IDENTIFIER '{' '}'
evalexpr : expr T_SEMICOLON ;
expr : literal ;
literal : T_BOOLEAN | T_FLOAT | T_STRING ;
vardecl : T_VAR varlist T_SEMICOLON ;
varlist : varcomp | varlist T_COMMA varcomp ;
varcomp : T_IDENTIFIER | T_IDENTIFIER T_EQUAL literal
%%

/* var_type : T_FLOAT | T_BOOLEAN | T_STRING ;
 */
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
