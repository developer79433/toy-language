/* scanner for a toy Pascal-like language */

%{
/* need this for the call to atof() below */
#include <math.h>
%}

%option noyywrap

%x C_COMMENT

DIGIT      [0-9]
IDENTIFIER [a-z][a-z0-9]*
SEMICOLON  ";"

%%

{DIGIT}+    {
            printf( "An integer: %s (%d)\n", yytext,
                    atoi( yytext ) );
            }

{DIGIT}+"."{DIGIT}*        {
            printf( "A float: %s (%g)\n", yytext,
                    atof( yytext ) );
            }

if|then|begin|end|procedure|function        {
            printf( "A keyword: %s\n", yytext );
            }

{IDENTIFIER}        printf( "An identifier: %s\n", yytext );

"+"|"-"|"*"|"/"|"="   printf( "An operator: %s\n", yytext );

";" printf( "Semicolon: %s\n", yytext );

"//"[^\n]*     /* eat C++-style one-line comments */

[ \t\n]+          printf( "Whitespace: \"%s\"\n", yytext );

.           printf( "Unrecognized character: %s\n", yytext );

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }

%%

int main(int argc, char **argv)
    {
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
            yyin = fopen( argv[0], "r" );
    else
            yyin = stdin;

    yylex();
    return 0;
    }
