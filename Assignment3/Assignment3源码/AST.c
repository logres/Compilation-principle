#include <stdio.h>
#include <string.h>

#include "AST.h"

NodePointer nonTerminalNode(int lineNo, TokenType type, char* nonTerminalName,int childNum, NodePointer* childs){
    TPointer T = (TPointer)malloc(sizeof(T));
    NodePointer node = (NodePointer)malloc(sizeof(node));
    node->element = T;
    T->lineNo = lineNo;
    T->name = nonTerminalName;
    T->type = type;

    node->firstChild=NULL;
    node->nextSib=NULL;

    node->firstChild = childs[0];
    NodePointer t = childs[0];
    for(int i = 1; i < childNum; i++){
        t->nextSib=childs[i];
        if (t->nextSib != NULL) {
            t = t->nextSib;
        }
    }
    return node;
}

char* strcopy(char* origin){
    int length = strlen(origin);
    char* str = (char*)malloc(sizeof(length+1));
    strcpy(str,origin);
    return str;
}

NodePointer terminalNode(int lineNo, TokenType type, char* terminalName,char* context){
    TPointer T = (TPointer)malloc(sizeof(T));
    NodePointer node = (NodePointer)malloc(sizeof(node));
    node->element = T;
    T->lineNo = lineNo;
    T->name = terminalName;
    T->type = type;
    
    T->val = strcopy(context);

    node->firstChild=NULL;
    node->nextSib=NULL;
    return node;
}

// 前序遍历

void printSpace(int count){
    for (int i =0; i<count;i++)
        printf("  ");
}

void printTree(NodePointer root,int count){
    printSpace(count);
    printf("%s",root->element->name);

    switch (root->element->type)
    {
    case TYPE_FLOAT:
        printf(": %f",atof(root->element->val));
        break;
    case TYPE_INT:
        printf(": %d",atoi(root->element->val));
        break;
    case TYPE_ID:
        printf(": %s",root->element->val);
        break;
    case TYPE_TYPE:
        printf(": %s",root->element->val);
        break;
    case TYPE_NONTERMINAL:
        printf(" (%d)",root->element->lineNo);
        break;
    default:
        break;
    }
    printf("\n");
    NodePointer temp = root->firstChild;
    while(temp!=NULL){
        printTree(temp,count+1);
        temp=temp->nextSib;
    }
}
