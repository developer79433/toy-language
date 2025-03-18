/* scanner for a toy Pascal-like language */

%{

enum yytokentype {
        T_BOOLEAN = 258,
        T_FLOAT,
        T_STRING,
        T_EQUAL,
        T_COMMA,
        T_COLON,
        T_ASTERISK,
        T_FSLASH,
        T_DOT,
        T_LPAREN,
        T_RPAREN,
        T_LBRACKET,
        T_RBRACKET,
        T_LBRACE,
        T_RBRACE,
        T_SEMICOLON,
        T_PLUS,
        T_MINUS,
        T_IF,
        T_ELSE,
        T_FOR,
        T_WHILE,
        T_VAR,
        T_FUN,
        T_AND,
        T_OR,
        T_NOT,
        T_IDENTIFIER
};

%}

%option noyywrap

%x C_COMMENT

DIGIT      [0-9]
IDENTIFIER [a-z][a-z0-9]*
SEMICOLON  ";"

%%

-?{DIGIT}+("."-?{DIGIT}*)?        { return T_FLOAT; }

["][^"]*["] { return T_STRING; }
['][^"]*['] { return T_STRING; }

if { return T_IF; }
else { return T_ELSE; }
for { return T_FOR; }
while { return T_WHILE; }
var { return T_VAR; }
fun { return T_FUN; }
and { return T_AND; }
or { return T_OR; }
not { return T_NOT; }

{IDENTIFIER}        { return T_IDENTIFIER; }

"=" { return T_EQUAL; }
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

"//"[^\n]*     /* eat C++-style single-line comments */

[ \t\n]+       /* eat whitespace */

.           printf( "Unrecognized character: %s\n", yytext );

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }

%%

