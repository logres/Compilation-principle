#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node{
	int line;	//行数
	char* name;	//节点名
	char* value;	//节点取值
	
	struct node* child;	//首孩子节点
	struct node* next;	//下个兄弟节点
} Node;

typedef Node* pNode;

static pNode createNode(int line, char* name, char* value, pNode child, pNode next){
	pNode newNode = NULL;
	int nameLength = strlen(name) + 1;
	int valueLength = strlen(value) + 1;
	
	newNode = (pNode)malloc(sizeof(Node));
	newNode->line = line;
	newNode->name = (char*)malloc(sizeof(char) * nameLength);
	newNode->value = (char*)malloc(sizeof(char) * valueLength);
	newNode->child = child;
	newNode->next = next;
	
	strncpy(newNode->name, name, nameLength);
	strncpy(newNode->value, value, valueLength);
	
	return newNode;
}

static void showNode(pNode node, int height){
	
	if(node == NULL) return;
	
	for(int i = 0; i < height; i++){
		printf("   ");
	}
	
	printf("%s", node->name);
	
	if(strcmp(node->name, "INT") == 0 ||
	   strcmp(node->name, "ID") == 0 ||
	   strcmp(node->name, "TYPE") == 0 ||
	   strcmp(node->name, "FLOAT") == 0){
	   	printf(": %s", node->value);
	}
	else if(strcmp(node->value, "") == 0){
		printf(" (%d)", node->line + 1);
	}	    
	
	printf("\n");
	
	showNode(node->child, height + 1);	
	showNode(node->next, height);
}

#endif

