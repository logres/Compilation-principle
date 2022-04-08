
%{

#include "syntax.tab.h"
#include "node.h"

extern int fault;

int yycolumn = 1;
#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno - 1; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
%}

%option yylineno

digit [0-9]
int [1-9]+[0-9]*|0
float {digit}+"."{digit}+|{digit}*"."{digit}+[eE][+-]?{digit}+|{digit}+"."{digit}+[eE][+-]?{digit}+|{digit}+

letter [_a-zA-Z]
ID {letter}[0-9_a-zA-Z]*

relop >|<|>=|<=|!=|==

%%

[ \t]+					{;}
\n|\r					{ yycolumn = 1;}

struct					{ yylval.node = createNode(yylineno, "STRUCT", yytext, NULL, NULL); return STRUCT;}
return 				{ yylval.node = createNode(yylineno, "RETURN", yytext, NULL, NULL); return RETURN;}
if 					{ yylval.node = createNode(yylineno, "IF", yytext, NULL, NULL); return IF;}
else 					{ yylval.node = createNode(yylineno, "ELSE", yytext, NULL, NULL); return ELSE;}
while 					{ yylval.node = createNode(yylineno, "WHILE", yytext, NULL, NULL); return WHILE;}
int|float				{ yylval.node = createNode(yylineno, "TYPE", yytext, NULL, NULL); return TYPE;}

{ID}					{ yylval.node = createNode(yylineno, "ID", yytext, NULL, NULL); return ID; printf("\nID\n");}
{int} 					{ yylval.node = createNode(yylineno, "INT", yytext, NULL, NULL); return INT; printf("\nINT\n");}
{float}	 			{ yylval.node = createNode(yylineno, "FLOAT", yytext, NULL, NULL); return FLOAT;}

";" 					{ yylval.node = createNode(yylineno, "SEMI", yytext, NULL, NULL); return SEMI; printf("\nSEMI\n");}
"," 					{ yylval.node = createNode(yylineno, "COMMA", yytext, NULL, NULL); return COMMA;}
"=" 					{ yylval.node = createNode(yylineno, "ASSIGNOP", yytext, NULL, NULL); return ASSIGNOP; printf("\nASSIGNOP\n");}
{relop}		 		{ yylval.node = createNode(yylineno, "RELOP", yytext, NULL, NULL); return RELOP;}
"+" 					{ yylval.node = createNode(yylineno, "PLUS", yytext, NULL, NULL); return PLUS;}
"-" 					{ yylval.node = createNode(yylineno, "MINUS", yytext, NULL, NULL); return MINUS;}
"*" 					{ yylval.node = createNode(yylineno, "STAR", yytext, NULL, NULL); return STAR;}
"/" 					{ yylval.node = createNode(yylineno, "DIV", yytext, NULL, NULL); return DIV;}
"&&" 					{ yylval.node = createNode(yylineno, "AND", yytext, NULL, NULL); return AND;}
"||" 					{ yylval.node = createNode(yylineno, "OR", yytext, NULL, NULL); return OR;}
"." 					{ yylval.node = createNode(yylineno, "DOT", yytext, NULL, NULL); return DOT;}
"!" 					{ yylval.node = createNode(yylineno, "NOT", yytext, NULL, NULL); return NOT;}
"(" 					{ yylval.node = createNode(yylineno, "LP", yytext, NULL, NULL); return LP;}
")" 					{ yylval.node = createNode(yylineno, "RP", yytext, NULL, NULL); return RP;}
"[" 					{ yylval.node = createNode(yylineno, "LB", yytext, NULL, NULL); return LB;}
"]" 					{ yylval.node = createNode(yylineno, "RB", yytext, NULL, NULL); return RB;}
"{" 					{ yylval.node = createNode(yylineno, "LC", yytext, NULL, NULL); return LC;}
"}" 					{ yylval.node = createNode(yylineno, "RC", yytext, NULL, NULL); return RC;}

.					{ printf("Error type A at Line %d: Mysterious character \'%s\'.\n", yylineno, yytext);}

%%

/*
int main(int argc, char** argv) {
    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            perror(argv[1]);
            return 1;
        }
    }
    yylex();
    return 0;
}
*/

int yywrap(){
	return 1;
}


