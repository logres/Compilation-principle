#ifndef AST_H
#define AST_H

#include <stdio.h>
#include "TokenType.h"
#include "stdlib.h"

typedef struct{
    int lineNo;
    char* name;
    TokenType type;
    char*  val;
}T;

typedef T* TPointer;

typedef struct node{
    TPointer element;
    struct node* firstChild;
    struct node* nextSib;
}Node;

typedef Node* NodePointer;

NodePointer nonTerminalNode(int lineNo, TokenType type, char* nonTerminalName,int childNum, NodePointer* childs);
NodePointer terminalNode(int lineNo, TokenType type, char* terminalName,char* context);
void printTree(NodePointer,int);

#endif