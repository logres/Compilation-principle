%{
#include "AST.h"
#include "flex.c"
#include "Mysemantic.h"
#include "InterCode.h"


extern int flag;
extern int yylineno;
extern char* yytext;
int is_debug=0;
#include <stdio.h>
char* symbol="";
void yyerror(char* msg){
    printf("Error type B at line %d:",yylineno);
    flag=1;
}
NodePointer root;

%}

%locations

%union{
    NodePointer node;
}

%token <node> INT
%token <node> FLOAT
%token <node> ID 
%token <node> SEMI COMMA ASSIGNOP
%token <node> RELOP PLUS MINUS STAR
%token <node> DIV AND OR DOT NOT
%token <node> TYPE LP RP LB RB LC RC
%token <node> STRUCT RETURN IF ELSE WHILE

%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag 
%type <node> VarDec FunDec VarList ParamDec   
%type <node> CompSt StmtList Stmt     
%type <node> DefList Def Dec DecList   
%type <node> Exp Args   



/*Token Declaration*/
           

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left UMINUS
%left LP RP LB RB DOT



%%

/*High Level Definitions*/

Program:ExtDefList                              {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Program",1,childs);root=$$;}
    ;
ExtDefList:ExtDef ExtDefList                    {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"ExtDefList",2,childs);}
    | /*empty*/                                 {   $$=NULL;}
    ;
ExtDef:Specifier ExtDecList SEMI                {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"ExtDef",3,childs);}
    |Specifier SEMI                             {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"ExtDef",2,childs);}
    |Specifier FunDec CompSt                    {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"ExtDef",3,childs);}
    |error SEMI                                 {   printf(" Single \";\" alone.\n");}
    ;
ExtDecList:VarDec                               {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"ExtDecList",1,childs);}
    |VarDec COMMA ExtDecList                    {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"ExtDecList",3,childs);}
    ;

/*Specifier*/

Specifier:TYPE                                  {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Specifier",1,childs);}
    |StructSpecifier                            {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Specifier",1,childs);}
    ;
StructSpecifier:STRUCT OptTag LC DefList RC     {   NodePointer childs[5];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;childs[4]=$5;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"StructSpecifier",5,childs);}
    |STRUCT Tag                                 {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"StructSpecifier",2,childs);}
    ;
OptTag:ID                                       {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"OptTag",1,childs);}
    |/*empty*/                                  {   $$=NULL;}
    ;
Tag:ID                                          {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Tag",1,childs);}
    ;

/*Declarator*/

VarDec:ID                                       {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"VarDec",1,childs);}
    |VarDec LB INT RB                           {   NodePointer childs[4];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"VarDec",4,childs);}
    ;
FunDec:ID LP VarList RP                         {   NodePointer childs[4];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"FunDec",4,childs);}
    |ID LP RP                                   {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"FunDec",3,childs);}
    ;
VarList:ParamDec COMMA VarList                  {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"VarList",3,childs);}
    |ParamDec                                   {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"VarList",1,childs);}
    ;
ParamDec:Specifier VarDec                       {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"ParamDec",2,childs);}
    ;

/*Statements*/

CompSt:LC DefList StmtList RC                   {   NodePointer childs[4];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"CompSt",4,childs);}
    |error RC                                   {}
    ;
StmtList:Stmt StmtList                          {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"StmtList",2,childs);}
    | /*empty*/                                 {   $$=NULL;}
    ;
Stmt:Exp SEMI                                   {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Stmt",2,childs);}
    |CompSt                                     {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Stmt",1,childs);}
    |RETURN Exp SEMI                            {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Stmt",3,childs);}
    |IF LP Exp RP Stmt %prec LOWER_THAN_ELSE    {   NodePointer childs[5];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;childs[4]=$5;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Stmt",5,childs);}
    |IF LP Exp RP Stmt ELSE Stmt                {   NodePointer childs[7];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;childs[4]=$5;childs[5]=$6;childs[6]=$7;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Stmt",7,childs);}
    |WHILE LP Exp RP Stmt                       {   NodePointer childs[5];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;childs[4]=$5;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Stmt",5,childs);}
    |error SEMI                                 {   printf(" Missing \";\".\n");}
    ;

/*Local Definition*/

DefList:Def DefList                             {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"DefList",2,childs);}
    | /*empty*/                                 {   $$=NULL;}
    ;
Def:Specifier DecList SEMI                      {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Def",3,childs);}
    ;
DecList:Dec                                     {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"DecList",1,childs);}
    |Dec COMMA DecList                          {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"DecList",3,childs);}
    ;   
Dec:VarDec                                      {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Dec",1,childs);}
    |VarDec ASSIGNOP Exp                        {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Dec",3,childs);}
    ;

/*Expressions*/

Exp:Exp ASSIGNOP Exp                            {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |Exp AND Exp                                {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |Exp OR Exp                                 {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |Exp RELOP Exp                              {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |Exp PLUS Exp                               {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |Exp MINUS Exp                              {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |Exp STAR Exp                               {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |Exp DIV Exp                                {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |LP Exp RP                                  {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |MINUS Exp %prec UMINUS                     {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",2,childs);}
    |NOT Exp                                    {   NodePointer childs[2];childs[0]=$1;childs[1]=$2;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",2,childs);}
    |ID LP Args RP                              {   NodePointer childs[4];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",4,childs);}    
    |ID LP RP                                   {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |Exp LB Exp RB                              {   NodePointer childs[4];childs[0]=$1;childs[1]=$2;childs[2]=$3;childs[3]=$4;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",4,childs);}
    |Exp DOT ID                                 {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",3,childs);}
    |ID                                         {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",1,childs);}
    |INT                                        {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",1,childs);}
    |FLOAT                                      {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Exp",1,childs);}
    |Exp LB Exp error RB                        {   printf(" Missing \"]\".\n");} 
    |error RP                                   {}
    ;
Args:Exp COMMA Args                             {   NodePointer childs[3];childs[0]=$1;childs[1]=$2;childs[2]=$3;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Args",3,childs);}
    |Exp                                        {   NodePointer childs[1];childs[0]=$1;$$ = nonTerminalNode(@$.first_line,TYPE_NONTERMINAL,"Args",1,childs);}
    ;

%%

int main(int argc, char** argv)
{

    verbose = 0;
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
    perror(argv[1]);
    return 1;
    }
    yyrestart(f);
    //printf("Start\n");
    yydebug=0;
    yyparse();
    
    table = newSymbolTable();
    init();
    traverseTree(root,0);
    //printTable(table);
    
    codeRoot=NULL;
    traverseTreeForTranslate(root);

    print_code(codeRoot);
    /* printf("Input file name:\n");
    char s[100];
    scanf("%s",&s);
    if(strcmp(s,"console")){
        freopen(s,"w",stdout);
        print_code(codeRoot);
        fclose(stdout);
    }else{
        print_code(codeRoot);
    } */
    
    

    return 0;
}

