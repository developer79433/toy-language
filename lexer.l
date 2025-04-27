/* scanner for a toy Pascal-like language */

%{

#include <stdio.h>
#include <stdlib.h>

#include "toy-num-types.h"
#include "toy-str-types.h"
#include "toy-bool-types.h"
/* TODO: Can these be moved to parser.y? */
#include "expr-types.h"
#include "stmt-types.h"
#include "parser.tab.h"

%}

%option noyywrap
%option nounput
%option noinput

%x C_COMMENT

DIGIT      [0-9]
IDENTIFIER [a-z][a-z0-9A-Z_]*
SEMICOLON  ";"
TRUE (?i:true)
FALSE (?i:false)

%%

-?{DIGIT}+("."-?{DIGIT}*)?        { yylval.num = atof(yytext); return T_FLOAT; }

\"([^"\n]*)\" {
    yylval.str = (char *) malloc(strlen(yytext) - 1);
    strncpy(yylval.str, &yytext[1], strlen(yytext) - 2);
    yylval.str[strlen(yytext) - 1] = '\0';
    return T_STRING;
}
\'([^'\n]*)\' {
    yylval.str = (char *) malloc(strlen(yytext) - 1);
    strncpy(yylval.str, &yytext[1], strlen(yytext) - 2);
    yylval.str[strlen(yytext) - 1] = '\0';
    return T_STRING;
}

{TRUE} { yylval.bool = TOY_TRUE; return T_BOOLEAN; }
{FALSE} { yylval.bool = TOY_FALSE; return T_BOOLEAN; }
if { return T_IF; }
else { return T_ELSE; }
elseif { return T_ELSEIF; }
for { return T_FOR; }
while { return T_WHILE; }
var { return T_VAR; }
fun(ction)? { return T_FUN; }
and { return T_AND; }
"&&" { return T_AND; }
or { return T_OR; }
"||" { return T_OR; }
not { return T_NOT; }
"!" { return T_NOT; }
in { return T_IN; }
return { return T_RETURN; }
break { return T_BREAK; }
continue { return T_CONTINUE; }
null { return T_NULL; }

{IDENTIFIER}        { yylval.str = strdup(yytext); return T_IDENTIFIER; }

"**" { return T_ASTERISK_ASTERISK; }
"++" { return T_PLUS_PLUS; }
"--" { return T_MINUS_MINUS; }
"!=" { return T_NEQUAL; }
"==" { return T_EQUAL; }
"<" { return T_LT; }
">" { return T_GT; }
"<=" { return T_LTE; }
">=" { return T_GTE; }
"=" { return T_ASSIGN; }
":" { return T_COLON; }
";" { return T_SEMICOLON; }
"," { return T_COMMA; }
"(" { return T_LPAREN; }
")" { return T_RPAREN; }
"[" { return T_LBRACKET; }
"]" { return T_RBRACKET; }
"{" { return T_LBRACE; }
"}" { return T_RBRACE; }
"+" { return T_PLUS; }
"-" { return T_MINUS; }
"*" { return T_ASTERISK; }
"/" { return T_FSLASH; }
"." { return T_DOT; }
"%" { return T_PERCENT; }
"?" { return T_QUESTION; }

"//"[^\n]*     /* eat C++-style single-line comments */

[ \t\n]+       /* eat whitespace */

.           { fprintf(stderr, "Unrecognized character: %s\n", yytext); abort(); }

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }

%%

void init_lexer(FILE *f)
{
#ifdef YYDEBUG
    yy_flex_debug = 1;
#define YYERROR_VERBOSE
#endif
    yyin = f;
}
