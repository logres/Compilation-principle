%{
#include "bison.h" 
int flag=0;
int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno;\
    yylloc.first_column = yycolumn;\
    yylloc.last_column = yycolumn + yyleng - 1;\
    yycolumn += yyleng;
%}

%option yylineno


space ([ \t])
digit ([0-9])
letter ([_a-zA-Z])
SEMI (";")
COMMA (",")
ASSIGNOP (=)
RELOP (>|<|>=|<=|==|!=)
PLUS ("+")
MINUS ("-")
STAR ("*")
DIV ("/")
AND ("&&")
OR ("||")
DOT (".")
NOT ("!")
TYPE (int|float)
LP ("(")
RP (")")
LB ("[")
RB ("]")
LC ("{")
RC ("}")
STRUCT ("struct")
RETURN ("return")
IF ("if")
ELSE ("else")
WHILE ("while")
INT ([+-]?{digit}*)
FLOAT ({digit}+"."{digit}+|{digit}*"."{digit}+[eE][+-]?{digit}+|{digit}+"."{digit}*[eE][+-]?{digit}+)
ID ({letter}({letter}|{digit})*)
%%
{space}     {;}
{SEMI}      {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"SEMI",yytext);return SEMI;}
{COMMA}     {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"COMMA",yytext);return COMMA;}
{ASSIGNOP}  {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"ASSIGNOP",yytext);return ASSIGNOP;}
{RELOP}     {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"RELOP",yytext);return RELOP;}
{PLUS}      {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"PLUS",yytext);return PLUS;}
{MINUS}     {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"MINUS",yytext);return MINUS;}
{STAR}      {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"STAR",yytext);return STAR;}
{DIV}       {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"DIV",yytext);return DIV;}
{AND}       {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"AND",yytext);return AND;}
{OR}        {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"OR",yytext);return OR;}
{DOT}       {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"DOT",yytext);return DOT;}
{NOT}       {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"NOT",yytext);return NOT;}
{TYPE}      {   yylval.node=terminalNode(yylineno,TYPE_TYPE,"TYPE",yytext);return TYPE;}
{LP}        {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"LP",yytext);return LP;}
{RP}        {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"RP",yytext);return RP;}
{LB}        {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"LB",yytext);return LB;}
{RB}        {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"RB",yytext);return RB;}
{LC}        {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"LC",yytext);return LC;}
{RC}        {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"RC",yytext);return RC;}
{STRUCT}    {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"STRUCT",yytext);return STRUCT;}
{RETURN}    {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"RETURN",yytext);return RETURN;}
{IF}        {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"IF",yytext);return IF;}
{ELSE}      {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"ELSE",yytext);return ELSE;}
{WHILE}     {   yylval.node=terminalNode(yylineno,TYPE_OTHER,"WHILE",yytext);return WHILE;}
{ID}        {   yylval.node=terminalNode(yylineno,TYPE_ID,"ID",yytext);return ID;}
{INT}       {   yylval.node=terminalNode(yylineno,TYPE_INT,"INT",yytext);return INT;}
{FLOAT}     {   yylval.node=terminalNode(yylineno,TYPE_FLOAT,"FLOAT",yytext);return FLOAT;}
\n|\r       {yycolumn=1;}
.           {flag=1;printf("Error type A at Line %d Column %d: Mysterious character \"%s\".\n",yylineno,yycolumn,yytext);}
%%
int yywrap() { 
    return 1;
}