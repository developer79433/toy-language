/* scanner for a toy Pascal-like language */

%{

enum yytokentype {
        T_BOOLEAN = 258,
        T_FLOAT,
        T_STRING,
        T_OPERATOR,
        T_EQUAL,
        T_COMMA,
        T_SEMICOLON,
        T_IF,
        T_FOR,
        T_WHILE,
        T_VAR,
        T_FUN,
        T_IDENTIFIER
};

%}

%option noyywrap

%x C_COMMENT

DIGIT      [0-9]
IDENTIFIER [a-z][a-z0-9]*
SEMICOLON  ";"

%%

{DIGIT}+("."{DIGIT}*)?        { return T_FLOAT; }

if { return T_IF; }
for { return T_FOR; }
while { return T_WHILE; }
var { return T_VAR; }
fun { return T_FUN; }

{IDENTIFIER}        { return T_IDENTIFIER; }

"+"|"-"|"*"|"/"|"."   { return T_OPERATOR; }

"=" { return T_EQUAL; }
";" { return T_SEMICOLON; }
"," { return T_COMMA; }

"//"[^\n]*     /* eat C++-style single-line comments */

[ \t\n]+       /* eat whitespace */

.           printf( "Unrecognized character: %s\n", yytext );

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }

%%

