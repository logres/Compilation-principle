#ifndef INTERCODE_H
#define INTERCODE_H

#include "AST.h"

typedef enum {OP_VARIABLE, OP_CONSTANT, OP_ADDRESS, OP_TEMP, OP_LABEL, OP_FUNCTION, OP_ARRAY, OP_STRUCTURE, OP_RETURN_ADDRESS, OP_RELOP} OperandKind;

typedef struct Operand {    //计算数据类型
    OperandKind kind;
    union {
        //int var_no;有无必要？
        int value;
        char* name;
        
    } detail;
    char* symbol;
}Operand;

typedef struct Operand* OperandPointer;

typedef enum {Code_ASSIGN, Code_ADD, Code_SUB, Code_MUL, Code_DIV, Code_LABEL, Code_GOTO, Code_IF, Code_DEC, Code_ARG, Code_CALL, Code_PARAM, Code_READ, Code_WRITE, Code_RETURN, Code_FUNCTION} InstructionKind;

typedef struct InterCode {   //中间代码类型
    InstructionKind kind;
    union 
    {
        struct {OperandPointer right, left;} assign;//赋值
        struct {OperandPointer result, op1, op2;} binop;//双目
        struct {OperandPointer op;} unop;//单目
        struct {OperandPointer op;} label;//跳转标签
        struct {OperandPointer op;} goto_;//无条件跳转
        struct {OperandPointer op1, relop,op2,label;} if_;//条件跳转
        struct {OperandPointer op;int size;} dec;//变量声明
        struct {OperandPointer op;} arg;//参数传递
        struct {OperandPointer result,func;} call;//函数调用
        struct {OperandPointer op;} param;//函数参数传递
        struct {OperandPointer op;} read;//读取
        struct {OperandPointer op;} write;//写入
        struct {OperandPointer op;} return_;//返回
        struct {OperandPointer op;} func;//函数声明
    } detail;
}InterCode;

typedef InterCode* InterCodePointer;

typedef struct InterCodes { //双向链表，中间代码表
    InterCodePointer code;
    struct InterCodes* prev, *next;
}InterCodes;

typedef InterCodes* InterCodesPointer;

typedef struct argsList {
    OperandPointer arg;
    struct argsList *prev, *next;
}ArgsList;

typedef ArgsList* ArgsListPointer;

typedef struct argCodetuple{
    ArgsListPointer arglist;
    InterCodesPointer code;
}ArgCodetuple;

typedef ArgCodetuple* ArgCodetuplePointer;

ArgCodetuplePointer new_argcodetuple(ArgsListPointer arglist, InterCodesPointer code);

//Arglist Function

ArgsListPointer new_argsList(OperandPointer arg);
ArgsListPointer contactArgsList(ArgsListPointer argslist1, ArgsListPointer argslist2);

//Operand Function
OperandPointer new_operand(OperandKind kind,...);
OperandPointer new_temp();
OperandPointer new_constant(int value);
OperandPointer new_label();
OperandPointer new_function(char* name);
OperandPointer new_variable(char* name);

typedef struct OperandList{
    OperandPointer op;
    struct OperandList *prev, *next;
}OperandList;

typedef OperandList* OperandListPointer;

OperandListPointer variableList;

//InterCode Function
InterCodePointer new_intercode(InstructionKind kind,...);

//InterCodes Function
InterCodesPointer new_intercodes(InterCodePointer);

//General Function
void print_code(InterCodesPointer codes);
void traverseTreeForTranslate(NodePointer root);
void print_op(OperandPointer op);


InterCodesPointer translate_Exp(NodePointer, OperandPointer*);
InterCodesPointer translate_Stmt(NodePointer);
InterCodesPointer translate_Cond(NodePointer, OperandPointer, OperandPointer);
ArgCodetuplePointer translate_Args(NodePointer, ArgsListPointer);

void translate_ExtDef(NodePointer);
void translate_ExtDefList(NodePointer);

void translate_ExtDecList(NodePointer);

void translate_Specifier(NodePointer);
void translate_StructSpecifier(NodePointer);
void translate_VarDec(NodePointer);
InterCodesPointer translate_FunDec(NodePointer);
void translate_VarList(NodePointer);
void translate_ParamDec(NodePointer);
InterCodesPointer translate_CompSt(NodePointer);
void translate_StmtList(NodePointer);
void translate_DefList(NodePointer);
void translate_Def(NodePointer);
void translate_DecList(NodePointer);
void translate_Dec(NodePointer);
void translate_OptTag(NodePointer);
void translate_Tag(NodePointer);

InterCodesPointer codeRoot;
extern InterCodesPointer codeRoot;

#endif