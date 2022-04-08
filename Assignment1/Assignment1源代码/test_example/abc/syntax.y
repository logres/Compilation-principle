
%code requires {
#include "node.h"
}

%{
#include <stdio.h>
#include "lex.yy.c"
int yydebug = 1;	//调试用 -v

pNode root;	//根节点
int fault = 0;	//错误标记

void yyerror(char* msg){
	fprintf(stderr, "Error type B at line %d: %s.\n", yylineno, msg);
}

int yylex();

%}

%locations

%union {
	pNode node;
}

%token <node> INT FLOAT
%token <node> ID LP RP LB RB LC RC SEMI COMMA ASSIGNOP DOT
%token <node> IF ELSE RETURN STRUCT WHILE TYPE 
%token <node> AND OR RELOP PLUS MINUS STAR DIV NOT

%type <node> Program ExtDefList ExtDef
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UPPER_THAN_MINUS
%left DOT LC RC LB RB LP RP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE 

%%


/*High-level Definitions*/

Program : 			ExtDefList				{$$ = createNode(@$.first_line, "Program", "", $1, NULL);
									 root = $$;}
	;
ExtDefList : 			/* empty */				{$$ = NULL;}
	| 			ExtDef ExtDefList			{$$ = createNode(@$.first_line, "ExtDefList", "", $1, NULL);
									 $1->next = $2;}
	;
ExtDef : 			Specifier ExtDefList SEMI		{$$ = createNode(@$.first_line, "ExtDef", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Specifier SEMI				{$$ = createNode(@$.first_line, "ExtDef", "", $1, NULL);
									 $1->next = $2;}
	| 			Specifier FunDec CompSt		{$$ = createNode(@$.first_line, "ExtDef", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	|			error SEMI	/*error*/		{fault == 1;}
	
	;
ExtDefList : 			VarDec				 	{$$ = createNode(@$.first_line, "ExtDefList", "", $1, NULL);}

	| 			VarDec COMMA ExtDefList		{$$ = createNode(@$.first_line, "ExtDefList", "", $1, NULL);
									$1->next = $2; $2->next = $3;}
	;


	
/*Specifiers*/
	
Specifier : 			TYPE					{$$ = createNode(@$.first_line, "Specifier", "", $1, NULL);}

	| 			StructSpecifier			{$$ = createNode(@$.first_line, "Specifier", "", $1, NULL);}
	
	;
StructSpecifier : 		STRUCT OptTag LC DefList RC		{$$ = createNode(@$.first_line, "StructSpecifier", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4; $4->next = $5;}
	| 			STRUCT Tag 				{$$ = createNode(@$.first_line, "StructSpecifier", "", $1, NULL);
									 $1->next = $2;}
	;
OptTag : 			/* empty */				{$$ = NULL;}
	| 			ID					{$$ = createNode(@$.first_line, "OptTag", "", $1, NULL);}
	
	;
Tag : 				ID					{$$ = createNode(@$.first_line, "Tag", "", $1, NULL);}

	;
	
	
	
/*Declarators*/
	
VarDec : 			ID					{$$ = createNode(@$.first_line, "VarDec", "", $1, NULL);}

	| 			VarDec LB INT RB			{$$ = createNode(@$.first_line, "VarDec", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4;}
	|			error RB	/*error*/		{fault == 1;}
	
	;
FunDec : 			ID LP VarList RP			{$$ = createNode(@$.first_line, "FunDec", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4;}
	| 			ID LP RP				{$$ = createNode(@$.first_line, "FunDec", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	|			error RP	/*error*/		{fault == 1;}
	
	;
VarList : 			ParamDec COMMA VarList			{$$ = createNode(@$.first_line, "VarList", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			ParamDec			 	{$$ = createNode(@$.first_line, "VarList", "", $1, NULL);}
	
	;
ParamDec : 			Specifier VarDec			{$$ = createNode(@$.first_line, "ParamDec", "", $1, NULL);
									 $1->next = $2;}
	;
	
	
	
/*Statements*/	
	
CompSt : 			LC DefList StmtList RC			{$$ = createNode(@$.first_line, "CompSt", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4;}
	|			error RC	/*error*/		{fault == 1;}
	;
StmtList : 			/* empty */				{$$ = NULL;}
	| 			Stmt StmtList				{$$ = createNode(@$.first_line, "StmtList", "", $1, NULL);
									 $1->next = $2;}
	;
Stmt : 			Exp SEMI				{$$ = createNode(@$.first_line, "Stmt", "", $1, NULL);
									 $1->next = $2;}
	| 			CompSt					{$$ = createNode(@$.first_line, "Stmt", "", $1, NULL);
	}
	| 			RETURN Exp SEMI			{$$ = createNode(@$.first_line, "Stmt", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = createNode(@$.first_line, "Stmt", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4; $4->next = $5;}
	| 			IF LP Exp RP Stmt ELSE Stmt		{$$ = createNode(@$.first_line, "Stmt", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4; $4->next = $5; 
									 $5->next = $6; $6->next = $7;}
	| 			WHILE LP Exp RP Stmt			{$$ = createNode(@$.first_line, "Stmt", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4; $4->next = $5;}
	|			error SEMI	/*error*/		{fault == 1;}
	
	;		
	
	
	
/*Local Definitions*/

DefList : 			/* empty */				{$$ = NULL;}
	| 			Def DefList				{$$ = createNode(@$.first_line, "DefList", "", $1, NULL);
									 $1->next = $2;}
	;
Def : 				Specifier DecList SEMI			{$$ = createNode(@$.first_line, "Def", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	;
DecList : 			Dec					{$$ = createNode(@$.first_line, "DecList", "", $1, NULL);}

	| 			Dec COMMA DecList			{$$ = createNode(@$.first_line, "DecList", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	;
Dec : 				VarDec					{$$ = createNode(@$.first_line, "Dec", "", $1, NULL);}

	| 			VarDec ASSIGNOP Exp			{$$ = createNode(@$.first_line, "Dec", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	;
	


/*Expressions*/

Exp : 				Exp ASSIGNOP Exp			{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Exp AND Exp				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Exp OR Exp				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Exp RELOP Exp				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Exp PLUS Exp				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Exp MINUS Exp				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Exp STAR Exp				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Exp DIV Exp				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			LP Exp RP				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			MINUS Exp %prec UPPER_THAN_MINUS	{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2;}
	| 			NOT Exp				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2;}
	| 			ID LP Args RP				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4;}
	| 			ID LP RP				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			Exp LB Exp RB				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3; $3->next = $4;}
	| 			Exp DOT ID				{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	| 			ID					{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);}
	
	| 			INT					{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);}
	
	| 			FLOAT					{$$ = createNode(@$.first_line, "Exp", "", $1, NULL);}
	
	|			error RB	/*error*/		{fault++;}
	
	|			error RP	/*error*/		{fault++;}
	;
Args : 			Exp COMMA Args				{$$ = createNode(@$.first_line, "Args ", "", $1, NULL);
									 $1->next = $2; $2->next = $3;}
	|			Exp			 		{$$ = createNode(@$.first_line, "Args ", "", $1, NULL);}
	
	;	
	
%%

int main(int argc, char** argv){
	if(argc <= 1){			//参数错误
		yyparse();
		return 1;
	}
	
	FILE* f = fopen(argv[1], "r");
	if(!f){
		return 1;		//无法打开文件
	}
	
	yyrestart(f);
	
	//yydebug = 1;
		
	yyparse();
	
	if(fault == 0) showNode(root, 0);
	
	return 0;
}



