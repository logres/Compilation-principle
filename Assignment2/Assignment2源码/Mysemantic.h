#ifndef MYSEMANTIC_H
#define MYSEMENTIC_H

#include "TokenType.h"
#include "AST.h"
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"

int verbose;

//定义布尔类型
typedef enum { FALSE=0,TRUE=1} Boolean;

//定义Symbol类型
typedef enum { STRUCT_SYMBOL, VARIABLE_SYMBOL} SymbolKind;

//定义某个符号的大体类型：基本类型、数组类型、结构类型、函数类型
typedef enum {BASIC_KIND = 1, ARRAY_KIND, STRUCT_KIND, FUNCTION_KIND} Kind;
//基本类型分为INT与FLOAT
typedef enum {INT_KIND = 1, FLOAT_KIND} BasicKind;

typedef struct type* TypePointer;

//复合符号类型定义
typedef struct multiField {
    char* name;       // 域的名字
    int loc;
    TypePointer type;       //Pointer 域的类型
    struct multiField* nextField;  // 下一个域
} MultiField;

typedef MultiField* MultiFieldPointer;

//包含大体类型与具体内容的符号类型描述
typedef struct type {
    Kind kind;
    union {
        // 基本类型
        BasicKind basicType;
        // 数组类型信息包括元素类型与数组大小构成
        struct {
            TypePointer elem;
            int size;
        } array;
        // 结构体类型信息是一个链表
        struct {
            char * tag;
            MultiFieldPointer field;
        } structure;
        struct {
            int argc;          // argument counter
            MultiFieldPointer argv;   // argumentPointer vector
            TypePointer returnType;//Pointer returnType
        } function;
    } detail;
} Type;

//定义符号类型
typedef struct Symbol {
    char* name;
    int loc;
    TypePointer type;
    SymbolKind symbolKind;
} Symbol;

typedef Symbol* SymbolPointer;

//定义哈希表的链表的结点
typedef struct hashListNode {
    SymbolPointer symbol;
    struct hashListNode* next;
} HashListNode;

typedef HashListNode* HashListNodePointer;

//定义哈希表
typedef struct hashTable {
    HashListNodePointer* hashList;
} HashTable;

typedef HashTable* HashTablePointer;

//定义符号表
typedef struct table {
    int unnamedStructNum;
    HashTablePointer hashTable;
} Table;

typedef Table* TablePointer;

// Type functions
TypePointer newType(Kind,int,...);
TypePointer copyType(TypePointer);
Boolean compareType(TypePointer,TypePointer);
TypePointer searchStruct(TablePointer,char*);
void printType(TypePointer type);

// FieldList functions

MultiFieldPointer newMultiField(char*, int, TypePointer);
void printMultiField(MultiFieldPointer);
MultiFieldPointer copyMultiField(MultiFieldPointer);
MultiFieldPointer multiFieldFromSymbol(SymbolPointer);
MultiFieldPointer mergeMultiField(MultiFieldPointer, MultiFieldPointer);
int countMultiField(MultiFieldPointer);
Boolean compareMultiField(MultiFieldPointer, MultiFieldPointer);
Boolean compareParam(MultiFieldPointer,MultiFieldPointer);
char * GetParam(MultiFieldPointer);
MultiFieldPointer searchField(MultiFieldPointer, char*);

SymbolPointer newSymbol(char*, int, TypePointer);
void setSymbolName(SymbolPointer, char*);
void setSymbolType(SymbolPointer, TypePointer);
void setSymbolLoc(SymbolPointer, int);
void printSymbol(SymbolPointer);

//HashListNode Function

HashListNodePointer newHashListNode(SymbolPointer);

//HashTable Function

HashTablePointer newHashTable();
SymbolPointer lookupSymbol(HashTablePointer, char*);

//SymbolTable Function

TablePointer newSymbolTable();
void addSymbol(TablePointer, SymbolPointer);
void addSymbolFromMultiField(TablePointer, MultiFieldPointer);
SymbolPointer searchSymbol(TablePointer, char*);
void printTable(TablePointer);

//General Function

void showError(int type, int loc, int argc, ...);

TablePointer table;
extern TablePointer table;

#define HASH_TABLE_SIZE 0x3fff

// 哈希函数
static inline unsigned int hash(char* name) {
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i = val & ~HASH_TABLE_SIZE)
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
    return val;
}

void traverseTree(NodePointer node,int depth);

// Generate symbol table functions
void ExtDef(NodePointer node, int depth);
void ExtDecList(NodePointer node, int depth, TypePointer type);
TypePointer Specifier(NodePointer node, int depth);
TypePointer StructSpecifier(NodePointer node, int depth);
SymbolPointer VarDec(NodePointer node, int depth, TypePointer type);
void FunDec(NodePointer node, int depth, TypePointer type);
MultiFieldPointer VarList(NodePointer node, int depth);
SymbolPointer ParamDec(NodePointer node, int depth);
void CompSt(NodePointer node, int depth, TypePointer type);
void StmtList(NodePointer node, int depth, TypePointer type);
void Stmt(NodePointer node, int depth, TypePointer type);
MultiFieldPointer DefList(NodePointer node, int depth);
MultiFieldPointer Def(NodePointer node, int depth);
MultiFieldPointer DecList(NodePointer node, int depth,TypePointer type);
SymbolPointer Dec(NodePointer node, int depth, TypePointer type);
SymbolPointer Exp(NodePointer node, int depth);
MultiFieldPointer Args(NodePointer node, int depth);
char* OptTag(NodePointer node, int depth);
char* Tag(NodePointer node, int depth);
#endif