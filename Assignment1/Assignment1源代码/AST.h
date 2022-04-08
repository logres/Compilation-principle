#ifndef AST_H
#define AST_H

#include <stdio.h>
#include "TokenType.h"

typedef struct{
    int lineNo;
    char* name;
    Type type;
    char*  val;
}T;

typedef T* TPointer;

typedef struct node{
    TPointer element;
    struct node* firstChild;
    struct node* nextSib;
}node;

typedef node* nodePointer;

nodePointer nonTerminalNode(int lineNo, Type type, char* nonTerminalName,int childNum, nodePointer* childs);
nodePointer terminalNode(int lineNo, Type type, char* terminalName,char* context);
void printTree(nodePointer,int);

#endif