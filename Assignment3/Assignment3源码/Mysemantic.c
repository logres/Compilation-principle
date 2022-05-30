#define NEXT(p) if(p==NULL) printf("FUCK NULL\n"); p=p->nextSib
#define EQUAL(p,type) !strcmp(p->element->name,type)
#define PrintToken(p,d) {simplePrint(p,d+1);NEXT(p);}

#define BASIC_ARGC 1
#define ARRAY_ARGC 2
#define STRUCT_ARGC 2
#define FUNC_ARGC 3

#include "Mysemantic.h"

//TypeFunction

TypePointer newType(Kind kind,int argc,...){

    TypePointer typePointer = (TypePointer) malloc(sizeof(Type));
    typePointer->kind = kind;
    va_list valist;
    switch(kind){
        case BASIC_KIND:
            va_start(valist,argc);
            typePointer->detail.basicType = va_arg(valist,BasicKind);
            break;
        case ARRAY_KIND:
            va_start(valist,argc);
            typePointer->detail.array.elem = va_arg(valist,TypePointer);
            typePointer->detail.array.size = va_arg(valist,int);
            break;
        case STRUCT_KIND:
            va_start(valist,argc);
            typePointer->detail.structure.tag = va_arg(valist,char*);
            typePointer->detail.structure.field = va_arg(valist,MultiFieldPointer);
            break;
        case FUNCTION_KIND:
            va_start(valist,argc);
            typePointer->detail.function.argc = va_arg(valist,int);
            typePointer->detail.function.argv = va_arg(valist,MultiFieldPointer);
            typePointer->detail.function.returnType = va_arg(valist,TypePointer);
            break;
    }
    va_end(valist);
    return typePointer;
}

TypePointer copyType(TypePointer src){
    TypePointer typePointer = (TypePointer) malloc(sizeof(Type));
    typePointer->kind = src->kind;
    switch(src->kind){
        case BASIC_KIND:
            typePointer->detail.basicType = src->detail.basicType;
            break;
        case ARRAY_KIND:
            typePointer->detail.array.elem = copyType(src->detail.array.elem);
            typePointer->detail.array.size = src->detail.array.size;
            break;
        case STRUCT_KIND:
            typePointer->detail.structure.tag=src->detail.structure.tag;
            typePointer->detail.structure.field = copyMultiField(src->detail.structure.field);
            break;
        case FUNCTION_KIND:
            typePointer->detail.function.argc = src->detail.function.argc;
            typePointer->detail.function.argv = copyMultiField(src->detail.function.argv);
            typePointer->detail.function.returnType = copyType(src->detail.function.returnType);
            break;
    }
    return typePointer;
}

Boolean compareType(TypePointer a, TypePointer b){
    if(a==NULL||b==NULL) return FALSE;
    if(a->kind != b->kind) return FALSE;
    switch(a->kind){
        case BASIC_KIND:
            return a->detail.basicType == b->detail.basicType;
        case ARRAY_KIND:
            return compareType(a->detail.array.elem,b->detail.array.elem) && a->detail.array.size == b->detail.array.size;
        case STRUCT_KIND:
            return !strcmp(a->detail.structure.tag,b->detail.structure.tag);
            //return compareMultiField(a->detail.structure.,b->detail.structure.field); 比较结构
        case FUNCTION_KIND:
            return compareMultiField(a->detail.function.argv,b->detail.function.argv) && compareType(a->detail.function.returnType,b->detail.function.returnType);
    }
    return FALSE;
}

TypePointer searchStruct(TablePointer table, char* tag){
        char* name = (char*)malloc(sizeof(char)*(strlen("struct ")+strlen(tag)+1));
        sprintf(name,"struct %s",tag);
        SymbolPointer symbol = searchSymbol(table,name);
        return symbol == NULL?NULL:symbol->type;
}

void printType(TypePointer p){
    if(p==NULL) return;
    switch(p->kind){
        case BASIC_KIND:
            printf("%s",p->detail.basicType==INT_KIND?"int":"float");
            break;
        case ARRAY_KIND:
            //printf("array of ");
            printType(p->detail.array.elem);
            printf("[%d]",p->detail.array.size);
            break;
        case STRUCT_KIND:
            printf("struct %s{",p->detail.structure.tag);
            printMultiField(p->detail.structure.field);
            printf("}");
            break;
        case FUNCTION_KIND:
            printf("function (");
            printMultiField(p->detail.function.argv);
            printf(")");
            printf("->");
            printType(p->detail.function.returnType);
            break;
    }
}

//MultiField Fuction

MultiFieldPointer newMultiField(char* name, int loc, TypePointer type){
    MultiFieldPointer multiFieldPointer = (MultiFieldPointer) malloc(sizeof(MultiField));
    multiFieldPointer->name = name;
    multiFieldPointer->loc = loc;
    multiFieldPointer->type = type;
    multiFieldPointer->nextField = NULL;
    return multiFieldPointer;
}

MultiFieldPointer copyMultiField(MultiFieldPointer src){
    if(src == NULL) return NULL;
    MultiFieldPointer multiFieldPointer = (MultiFieldPointer) malloc(sizeof(MultiField));
    multiFieldPointer->name = src->name;
    multiFieldPointer->loc = src->loc;
    multiFieldPointer->type = copyType(src->type);
    multiFieldPointer->nextField = copyMultiField(src->nextField);
    return multiFieldPointer;
}

void printMultiField(MultiFieldPointer p){
    if(p==NULL) return;
    printf("Name:%s ",p->name);
    printf("Type: ");
    printType(p->type);
    printf(" at line %d",p->loc);
    printf("; ");
    printMultiField(p->nextField);
}

MultiFieldPointer multiFieldFromSymbol(SymbolPointer symbol){
    if(symbol == NULL) return NULL;
    MultiFieldPointer multiFieldPointer = (MultiFieldPointer) malloc(sizeof(MultiField));
    multiFieldPointer->name = symbol->name;
    multiFieldPointer->loc = symbol->loc;
    multiFieldPointer->type = symbol->type;
    multiFieldPointer->nextField = NULL;
    return multiFieldPointer;
}

MultiFieldPointer mergeMultiField(MultiFieldPointer a, MultiFieldPointer b){
    if(a == NULL) return b;
    if(b == NULL) return a;
    MultiFieldPointer p = a;
    while(p->nextField != NULL) p = p->nextField;
    p->nextField = b;
    return a;
}

int countMultiField(MultiFieldPointer multiField){
    if(multiField == NULL) return 0;
    int count = 1;
    while(multiField->nextField != NULL){
        count++;
        multiField = multiField->nextField;
    }
    return count;
}

Boolean compareMultiField(MultiFieldPointer a, MultiFieldPointer b){
    if(a == NULL && b == NULL) return TRUE;
    if(a == NULL || b == NULL) return FALSE;
    if(strcmp(a->name,b->name) != 0) return FALSE;
    if(!compareType(a->type,b->type)) return FALSE;
    return compareMultiField(a->nextField,b->nextField);
}

Boolean compareParam(MultiFieldPointer a ,MultiFieldPointer b){
    if(a == NULL && b == NULL) return TRUE;
    if(a == NULL || b == NULL) return FALSE;
    if(!compareType(a->type,b->type)) return FALSE;
    return compareParam(a->nextField,b->nextField);
}

char * GetParam(MultiFieldPointer multiField){
    if(multiField == NULL) return NULL;
    char * param = (char *)malloc(sizeof(char)*100);
    strcpy(param,multiField->type->kind == BASIC_KIND?"int":"float");
    //printType(multiField->type);
    if(multiField->nextField != NULL){
        strcat(param,",");
        strcat(param,GetParam(multiField->nextField));
    }
    return param;
}

MultiFieldPointer searchField(MultiFieldPointer multiField, char* name){
    if(multiField == NULL) return NULL;
    if(strcmp(multiField->name,name) == 0) return multiField;
    return searchField(multiField->nextField,name);
}

//Symbol Fuction
SymbolPointer newSymbol(char* name, int loc,TypePointer type){
    SymbolPointer symbolPointer = (SymbolPointer) malloc(sizeof(Symbol));
    symbolPointer->name = name;
    symbolPointer->loc = loc;
    symbolPointer->type = type;
    return symbolPointer;
}

void setSymbolName(SymbolPointer symbol, char* name){
    symbol->name = name;
}


void setSymbolType(SymbolPointer symbol , TypePointer type){
    symbol->type = type;
}

void setSymbolLoc(SymbolPointer symbol, int loc){
    symbol->loc = loc;
}

void addSymbolFromMultiField(TablePointer table, MultiFieldPointer multiField){
    if(multiField == NULL) return;
    SymbolPointer symbol = newSymbol(multiField->name,multiField->loc,multiField->type);
    addSymbol(table,symbol);
    addSymbolFromMultiField(table,multiField->nextField);
}

SymbolPointer searchSymbol(TablePointer table, char* symbolName){
    if(table == NULL) return NULL;
    return lookupSymbol(table->hashTable,symbolName);
}

void printSymbol(SymbolPointer p){
    if(p==NULL) return;
    printf("Name: %s ",p->name);
    printf("Type: ");
    printType(p->type);
    printf(" at line %d",p->loc);
    printf(";");
}

//HashListNode Fuction

HashListNodePointer newHashListNode(SymbolPointer symbol){
    HashListNodePointer hashListNodePointer = (HashListNodePointer) malloc(sizeof(HashListNode));
    hashListNodePointer->symbol = symbol;
    hashListNodePointer->next = NULL;
    return hashListNodePointer;
}

//HashTable Fuction

HashTablePointer newHashTable(){
    HashTablePointer hashTablePointer = (HashTablePointer) malloc(sizeof(HashTable));
    hashTablePointer->hashList = (HashListNodePointer*)malloc(sizeof(HashListNodePointer)*HASH_TABLE_SIZE);
    return hashTablePointer;
}

SymbolPointer lookupSymbol(HashTablePointer hashtable, char* symbolName){
    if(hashtable == NULL) return NULL;
    unsigned int hashcode = hash(symbolName);
    HashListNodePointer hashListNode = hashtable->hashList[hashcode];
    while(hashListNode != NULL){
        if(strcmp(hashListNode->symbol->name,symbolName) == 0) return hashListNode->symbol;
        hashListNode = hashListNode->next;
    }
    return NULL;
}

//SymbolTable Function

TablePointer newSymbolTable(){
    TablePointer tablePointer = (TablePointer) malloc(sizeof(Table));
    tablePointer->hashTable = newHashTable();
    tablePointer->unnamedStructNum = 0;
    return tablePointer;
}

void addSymbol(TablePointer table, SymbolPointer symbol){
    if(table == NULL) return;
    
    if(!strcmp(symbol->name,"struct")){//symbol->name = "struct" + symbol->type->detail.structure.tag
        symbol->symbolKind = STRUCT_SYMBOL;
        symbol->name = (char*)malloc(sizeof(char)*(strlen("struct ")+strlen(symbol->type->detail.structure.tag)+1));
        sprintf(symbol->name,"struct %s",symbol->type->detail.structure.tag);
        if(!strcmp(symbol->type->detail.structure.tag,"unnamedStruct")){
        symbol->type->detail.structure.tag = (char*)malloc(sizeof(char)*(strlen("unnamedStruct")+10));
        sprintf(symbol->type->detail.structure.tag,"unnamedStruct%d",table->unnamedStructNum);
        table->unnamedStructNum++;
        }

        MultiFieldPointer p = symbol->type->detail.structure.field;
        MultiFieldPointer q = NULL;
        char* name = NULL;
        while(p != NULL){
            name = p->name;
            q = searchField(p->nextField,name);
            if(q!=NULL){
                showError(15,q->loc,1,q->name);
            }
            p = p->nextField;
        }

    }else{
        symbol->symbolKind = VARIABLE_SYMBOL;
    }

    if(searchSymbol(table,symbol->name)!=NULL){
        if(symbol->type->kind == FUNCTION_KIND){
            showError(4,symbol->loc,1,symbol->name);
        }else if(symbol->symbolKind==STRUCT_SYMBOL){
            showError(16,symbol->loc,1,symbol->name);
        }else{
            showError(3,symbol->loc,1,symbol->name);
        }
    }

    int index = hash(symbol->name);
    HashListNodePointer hashListNodePointer = newHashListNode(symbol);
    hashListNodePointer->next = table->hashTable->hashList[index];
    table->hashTable->hashList[index] = hashListNodePointer;
}

void printTable(TablePointer tablePointer){
    printf("===========================Symbol Table=========================\n");
    int i;
    int count = 1;
    for(i=0;i<HASH_TABLE_SIZE;i++){
        HashListNodePointer hashListNodePointer = tablePointer->hashTable->hashList[i];
        while(hashListNodePointer!=NULL){
            printf("%d. ",count);
            printSymbol(hashListNodePointer->symbol);
            printf("\n");
            count++;
            hashListNodePointer = hashListNodePointer->next;
        }
    }
    printf("===============================End==============================\n");
}

//General Function

void showError(int type, int loc, int argc,...){
    va_list valist;
    va_start(valist,argc);
    switch(type){
        case 1:
            printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",loc,va_arg(valist,char*));
            break;
        case 2:
            printf("Error type 2 at Line %d: Undefined function \"%s\".\n",loc,va_arg(valist,char*));
            break;
        case 3:
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",loc,va_arg(valist,char*));
            break;
        case 4:
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",loc,va_arg(valist,char*));
            break;
        case 5:
            printf("Error type 5 at Line %d: Type mismatched for assignment.\n",loc);
            break;
        case 6:
            printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",loc);
            break;
        case 7:
            printf("Error type 7 at Line %d: Type mismatched for operands.\n",loc);
            break;
        case 8:
            printf("Error type 8 at Line %d: Type mismatched for return.\n",loc);
            break;
        case 9:
            printf("Error type 9 at Line %d: Function \"%s %s\" is not applicable for arguments \"%s\".\n",loc,va_arg(valist,char*),va_arg(valist,char*),va_arg(valist,char*));
            break;
        case 10:
            printf("Error type 10 at Line %d: \"%s\" is not an array.\n",loc,va_arg(valist,char*));
            break;
        case 11:
            printf("Error type 11 at Line %d: \"%s\" is not a function.\n",loc,va_arg(valist,char*));
            break;
        case 12:
            printf("Error type 12 at Line %d: \"%s\" is not an integer.\n",loc,va_arg(valist,char*));
            break;
        case 13:
            printf("Error type 13 at Line %d: Illegal use of \".\".\n",loc);
            break;
        case 14:
            printf("Error type 14 at Line %d: Illegal use of \"%s\".\n",loc,va_arg(valist,char*));
            break;
        case 15:
            printf("Error type 15 at Line %d: Redefined field \"%s\".\n",loc,va_arg(valist,char*));
            break;
        case 16:
            printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",loc,va_arg(valist,char*));
            break;
        case 17:
            printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",loc,va_arg(valist,char*));
            break;
        default:
            printf("No ERROR");
    }
    va_end(valist);
}

void init(){
    SymbolPointer read =  newSymbol("read",0,newType(FUNCTION_KIND,FUNC_ARGC,0,NULL,newType(BASIC_KIND,BASIC_ARGC,INT_KIND)));
    SymbolPointer write = newSymbol("write",0,newType(FUNCTION_KIND,FUNC_ARGC,1,newMultiField("a",0,newType(BASIC_KIND,BASIC_ARGC,INT_KIND)),newType(BASIC_KIND,BASIC_ARGC,INT_KIND)));
    //printSymbol(read);
    addSymbol(table,read);
    addSymbol(table,write);
}

///////////////////////////////////////////////

void traverseTree(NodePointer node, int depth){
    if(!node)return;
    if(EQUAL(node,"ExtDef")){
        ExtDef(node,depth);
    }
    traverseTree(node->firstChild,depth+1);
    traverseTree(node->nextSib,depth+1);
}

// Generate symbol table functions


void simplePrint(NodePointer root, int count){
    if(verbose==1){
        for (int i =0; i<count;i++)
            printf("  ");

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
    }
}

// ExtDef -> Specifier ExtDecList SEMI
//         | Specifier SEMI
//         | Specifier FunDec CompSt
void ExtDef(NodePointer node,int depth){

    simplePrint(node,depth);

    NodePointer currentChild=node->firstChild;
    TypePointer type = Specifier(currentChild,depth+1);NEXT(currentChild);
    if(EQUAL(currentChild,"ExtDecList")){//Specifier ExtDecList SEMI 全局变量 OK
        ExtDecList(currentChild,depth+1,type);NEXT(currentChild);
        PrintToken(currentChild,depth);


    }else if(EQUAL(currentChild,"SEMI")){//Specifier SEMI OK
        PrintToken(currentChild,depth);

    }else if(EQUAL(currentChild,"FunDec")){//Specifier FunDec CompSt

        FunDec(currentChild,depth+1,type);NEXT(currentChild);

        CompSt(currentChild,depth+1,type);NEXT(currentChild);
    }
}

// ExtDecList -> VarDec
//             | VarDec COMMA ExtDecList
void ExtDecList(NodePointer node, int depth, TypePointer type){

    simplePrint(node,depth);

    NodePointer currentChild=node->firstChild;
    SymbolPointer symbol = VarDec(currentChild,depth+1,type);NEXT(currentChild);
    addSymbol(table,symbol);
    if(currentChild){//VarDec COMMA ExtDecList
        PrintToken(currentChild,depth);
        ExtDecList(currentChild,depth+1,type);
    }else{//VarDec
        
    }
}

// Specifier -> TYPE
//            | StructSpecifier
TypePointer Specifier(NodePointer node, int depth){

    simplePrint(node,depth);
    NodePointer currentChild = node->firstChild;
    if(EQUAL(currentChild,"TYPE")){
        BasicKind basicKind;
        if(!strcmp(currentChild->element->val,"int")){
            basicKind = INT_KIND;
        }else{
            basicKind = FLOAT_KIND;
        }    //“int” or “float”
        PrintToken(currentChild,depth);
        return newType(BASIC_KIND,BASIC_ARGC,basicKind);
    }else if(EQUAL(currentChild,"StructSpecifier")){
        return StructSpecifier(currentChild,depth+1);NEXT(currentChild);//Todo
    }
}

// StructSpecifier -> STRUCT OptTag LC DefList RC
//                  | STRUCT Tag
TypePointer StructSpecifier(NodePointer node, int depth){

    simplePrint(node,depth);

    NodePointer currentChild = node->firstChild;
    
    TypePointer type = NULL;

    int unnamedLoc = currentChild->element->lineNo; 
    char * structName = "";
    PrintToken(currentChild,depth);//Struct

    if(EQUAL(currentChild,"OptTag")||EQUAL(currentChild,"LC")){
        SymbolPointer symbol = newSymbol("",0,NULL);
        if(EQUAL(currentChild,"OptTag")){
            //setSymbolName(symbol,currentChild->firstChild->element->val);//结构体非匿名&假设&
            setSymbolLoc(symbol,currentChild->firstChild->element->lineNo);
            structName=OptTag(currentChild,depth);NEXT(currentChild); //OptTag
        }else{
            structName =  "unnamedStruct";
            setSymbolLoc(symbol,unnamedLoc);
        }
        PrintToken(currentChild,depth);//LC
        MultiFieldPointer multiField = NULL;
        if(EQUAL(currentChild,"DefList")){
            multiField = DefList(currentChild,depth+1);NEXT(currentChild);

        }

        PrintToken(currentChild,depth);//RC
        type = newType(STRUCT_KIND,STRUCT_ARGC,structName,multiField);
        setSymbolName(symbol,"struct");
        setSymbolType(symbol,type);
        addSymbol(table,symbol);
        return type;

    }else if(EQUAL(currentChild,"Tag")){//Tag …… 不能不管
        int temploc = currentChild->firstChild->element->lineNo;
        char* name = Tag(currentChild,depth+1);NEXT(currentChild);
        TypePointer type = searchStruct(table,name);
        
        
        if(type==NULL){
            showError(17,temploc,1,name);
            return newType(STRUCT_KIND,STRUCT_ARGC,"UndefinedStructure",NULL);
        }
        
        return  type;
    }
}

// VarDec -> ID
//         | VarDec LB INT RB
SymbolPointer VarDec(NodePointer node, int depth, TypePointer type){

    simplePrint(node,depth);

    NodePointer currentChild = node->firstChild;
    if(EQUAL(currentChild,"ID")){
        SymbolPointer Symbol = newSymbol("",0,NULL);
        setSymbolName(Symbol,currentChild->element->val);
        setSymbolLoc(Symbol,currentChild->element->lineNo);
        setSymbolType(Symbol,copyType(type));
        PrintToken(currentChild,depth)
        return Symbol;
    }else if(EQUAL(currentChild,"VarDec")){
        NodePointer p = currentChild;
        NEXT(p);NEXT(p);int num = atoi(p->element->val);
        
        TypePointer arrayType = newType(ARRAY_KIND,ARRAY_ARGC,type,num);
        SymbolPointer Symbol = VarDec(currentChild,depth+1,arrayType);NEXT(currentChild);
        PrintToken(currentChild,depth)//LB
        PrintToken(currentChild,depth)//INT
        PrintToken(currentChild,depth)//RB

        return Symbol;
    }
    return NULL;
    
}

// FunDec -> ID LP VarList RP
//         | ID LP RP
void FunDec(NodePointer node, int depth, TypePointer type){

    simplePrint(node,depth);

    NodePointer currentChild = node->firstChild;

    SymbolPointer symbol = newSymbol("",0,NULL);
    setSymbolName(symbol,currentChild->element->val);
    setSymbolLoc(symbol,currentChild->element->lineNo);

    MultiFieldPointer varlist = NULL;
    PrintToken(currentChild,depth);//ID
    PrintToken(currentChild,depth);//LP
    if(EQUAL(currentChild,"VarList")){
        varlist = VarList(currentChild,depth+1);NEXT(currentChild);
        PrintToken(currentChild,depth);
    }else if(EQUAL(currentChild,"RP")){
        PrintToken(currentChild,depth);
    }
    setSymbolType(symbol,newType(FUNCTION_KIND,FUNC_ARGC,countMultiField(varlist),varlist,type));
    addSymbol(table,symbol);
}

// VarList -> ParamDec COMMA VarList
//          | ParamDec
MultiFieldPointer VarList(NodePointer node, int depth){
    NodePointer currentChild = node->firstChild;

    SymbolPointer symbol = NULL;
    MultiFieldPointer multiField = NULL;
    symbol = ParamDec(currentChild,depth+1);NEXT(currentChild);
    multiField = multiFieldFromSymbol(symbol);

    if(currentChild){//VarList -> ParamDec COMMA VarList
        MultiFieldPointer temp = NULL;
        PrintToken(currentChild,depth);//COMMA
        temp = VarList(currentChild,depth+1);NEXT(currentChild);
        multiField = mergeMultiField(multiField,temp);
    }
    addSymbolFromMultiField(table,multiField);
    return multiField;
}

// ParamDec -> Specifier VarDec
SymbolPointer ParamDec(NodePointer node, int depth){

    simplePrint(node,depth);

    NodePointer currentChild = node->firstChild;

    TypePointer type =  Specifier(currentChild,depth+1);NEXT(currentChild);
    SymbolPointer symbol =  VarDec(currentChild,depth+1,type);NEXT(currentChild);

    return symbol;
}

// CompSt -> LC DefList StmtList RC
void CompSt(NodePointer node, int depth, TypePointer type){
    simplePrint(node,depth);

    NodePointer currentChild = node->firstChild;
    
    PrintToken(currentChild,depth);
    if(EQUAL(currentChild,"DefList")){
        MultiFieldPointer localVars = DefList(currentChild,depth+1);NEXT(currentChild); 
        addSymbolFromMultiField(table,localVars);
    }
    if(EQUAL(currentChild,"StmtList")){
        StmtList(currentChild,depth+1,type);NEXT(currentChild);
    }
    PrintToken(currentChild,depth);
}

// StmtList -> Stmt StmtList
//           | e
void StmtList(NodePointer node, int depth, TypePointer type){
    simplePrint(node,depth);

    NodePointer currentChild = node->firstChild;

    if(currentChild){
        Stmt(currentChild,depth+1,type);NEXT(currentChild);
        if(currentChild){
            StmtList(currentChild,depth+1,type);NEXT(currentChild);
        }
    }
}

// Stmt -> Exp SEMI
//       | CompSt
//       | RETURN Exp SEMI
//       | IF LP Exp RP Stmt
//       | IF LP Exp RP Stmt ELSE Stmt
//       | WHILE LP Exp RP Stmt
void Stmt(NodePointer node, int depth, TypePointer type){

    simplePrint(node,depth);

    NodePointer currentChild = node->firstChild;

    SymbolPointer symbol1 = NULL;
    TypePointer type1 = NULL;
    int temploc = 0;

    if(EQUAL(currentChild,"Exp")){
        Exp(currentChild,depth+1);NEXT(currentChild);
        PrintToken(currentChild,depth);
    }else if(EQUAL(currentChild,"CompSt")){
        CompSt(currentChild,depth+1,type);NEXT(currentChild);

    }else if(EQUAL(currentChild,"RETURN")){
        
        PrintToken(currentChild,depth);
        temploc = currentChild->element->lineNo;
        symbol1 = Exp(currentChild,depth+1);NEXT(currentChild);
        type1 = symbol1->type;
        if(compareType(type,type1)==FALSE){
            showError(8,temploc,0,"");
        }
        PrintToken(currentChild,depth);

    }else if(EQUAL(currentChild,"IF")){


        PrintToken(currentChild,depth);//IF
        PrintToken(currentChild,depth);//LP
        
        Exp(currentChild,depth+1);NEXT(currentChild);
        PrintToken(currentChild,depth);
        Stmt(currentChild,depth+1,type);NEXT(currentChild);

        if(currentChild){//ELSE
            PrintToken(currentChild,depth);
            Stmt(currentChild,depth+1,type);NEXT(currentChild);
        }
    }else if(EQUAL(currentChild,"WHILE")){
        PrintToken(currentChild,depth);
        PrintToken(currentChild,depth);
        Exp(currentChild,depth+1);NEXT(currentChild);
        PrintToken(currentChild,depth);
        Stmt(currentChild,depth+1,type);NEXT(currentChild);
    }
}

// DefList -> Def DefList
//          | e
MultiFieldPointer DefList(NodePointer node, int depth){

    if(node == NULL) return NULL;

    simplePrint(node,depth);
    NodePointer currentChild = node->firstChild;

    MultiFieldPointer multiField=NULL;
    multiField = Def(currentChild,depth+1);NEXT(currentChild);


    MultiFieldPointer multiField2 = DefList(currentChild,depth+1);
    multiField = mergeMultiField(multiField,multiField2);

    return multiField;

}

// Def -> Specifier DecList SEMI
MultiFieldPointer Def(NodePointer node, int depth){

    simplePrint(node,depth);

    NodePointer currentChild = node->firstChild;

    TypePointer type = Specifier(currentChild,depth+1);NEXT(currentChild);
    MultiFieldPointer multiField = DecList(currentChild,depth+1,type);NEXT(currentChild);
    PrintToken(currentChild,depth);

    return multiField;

}

// DecList -> Dec
//          | Dec COMMA DecList
MultiFieldPointer DecList(NodePointer node, int depth, TypePointer type){

    simplePrint(node,depth);
    NodePointer currentChild = node->firstChild;
    
    SymbolPointer symbol = NULL;
    MultiFieldPointer multiField = NULL;
    symbol = Dec(currentChild,depth+1,type);NEXT(currentChild);
    multiField = multiFieldFromSymbol(symbol);

    if(currentChild){
        MultiFieldPointer temp = NULL;
        PrintToken(currentChild,depth);//COMMA
        temp = DecList(currentChild,depth+1,type);NEXT(currentChild);
        multiField = mergeMultiField(multiField,temp);
    }
        
    return multiField;
}

// Dec -> VarDec
//      | VarDec ASSIGNOP Exp?
SymbolPointer Dec(NodePointer node, int depth, TypePointer type){

    simplePrint(node,depth);
    NodePointer currentChild = node->firstChild;
    
    SymbolPointer symbol = NULL;
    symbol= VarDec(currentChild,depth+1,type);NEXT(currentChild);
    

    if(currentChild){//VarDec ASSIGNOP 
        PrintToken(currentChild,depth);//ASSIGNOP
        Exp(currentChild,depth+1);NEXT(currentChild);
           //To Do 研究错误
    }

    return symbol;

}

// Exp -> Exp ASSIGNOP Exp
//      | Exp AND Exp
//      | Exp OR Exp
//      | Exp RELOP Exp
//      | Exp PLUS Exp
//      | Exp MINUS Exp
//      | Exp STAR Exp
//      | Exp DIV Exp
//      | LP Exp RP
//      | MINUS Exp
//      | NOT Exp
//      | ID LP Args RP
//      | ID LP RP
//      | Exp LB Exp RB
//      | Exp DOT ID
//      | ID
//      | INT
//      | FLOAT
SymbolPointer Exp(NodePointer node, int depth){
    SymbolPointer symbol1 = NULL;
    SymbolPointer symbol2 = NULL;
    TypePointer type1 = NULL;
    TypePointer type2 = NULL;
    char* name1 = NULL;
    char* name2 = NULL;
    int loc1 = 0;
    int loc2 = 0;
    NodePointer p = NULL;

    simplePrint(node,depth);
    NodePointer currentChild = node->firstChild;
    
    if(EQUAL(currentChild,"Exp")){

        symbol1 = Exp(currentChild,depth+1);
        name1 = symbol1->name;
        loc1 = symbol1->loc;
        type1 = symbol1->type;
        NEXT(currentChild);

        if(EQUAL(currentChild,"ASSIGNOP")){
            if(strcmp(name1,"int")==0||strcmp(name1,"float")==0){
                showError(6,loc1,0,"");
            }
            PrintToken(currentChild,depth);

            symbol2 = Exp(currentChild,depth+1);NEXT(currentChild);
            name2 = symbol2->name;
            loc2 = symbol2->loc;
            type2 = symbol2->type;
            if(type2->kind==FUNCTION_KIND){
                type2 = type2->detail.function.returnType;
            }

            if(compareType(type1,type2)==FALSE){
                showError(5,loc2,0,"");
            }
            return symbol1;

        }else if(EQUAL(currentChild,"AND")){
            PrintToken(currentChild,depth);
            Exp(currentChild,depth+1);NEXT(currentChild);
        }else if(EQUAL(currentChild,"OR")){
            PrintToken(currentChild,depth);
            Exp(currentChild,depth+1);NEXT(currentChild);
        }else if(EQUAL(currentChild,"RELOP")){
            PrintToken(currentChild,depth);
            Exp(currentChild,depth+1);NEXT(currentChild);
        }else if(EQUAL(currentChild,"PLUS")){
            
            PrintToken(currentChild,depth);
            symbol2 = Exp(currentChild,depth+1);NEXT(currentChild);
            name2 = symbol2->name;
            loc2 = symbol2->loc;
            type2 = symbol2->type;
            if(type2->kind==FUNCTION_KIND){
                type2 = type2->detail.function.returnType;
            }
            if(compareType(type1,type2)==FALSE){
                showError(7,loc1,0,"");
            }
            return symbol1;
        }else if(EQUAL(currentChild,"MINUS")){

            PrintToken(currentChild,depth);
            symbol2 = Exp(currentChild,depth+1);NEXT(currentChild);
            name2 = symbol2->name;
            loc2 = symbol2->loc;
            type2 = symbol2->type;
            if(type2->kind==FUNCTION_KIND){
                type2 = type2->detail.function.returnType;
            }
            if(compareType(type1,type2)==FALSE){
                showError(7,loc1,0,"");
            }
            return symbol1;
        }else if(EQUAL(currentChild,"STAR")){

            PrintToken(currentChild,depth);
            symbol2 = Exp(currentChild,depth+1);NEXT(currentChild);
            name2 = symbol2->name;
            loc2 = symbol2->loc;
            type2 = symbol2->type;
            if(type2->kind==FUNCTION_KIND){
                type2 = type2->detail.function.returnType;
            }
            if(compareType(type1,type2)==FALSE){
                showError(7,loc1,0,"");
            }
            return symbol1;
        }else if(EQUAL(currentChild,"DIV")){
            PrintToken(currentChild,depth);
            symbol2 = Exp(currentChild,depth+1);NEXT(currentChild);
            name2 = symbol2->name;
            loc2 = symbol2->loc;
            type2 = symbol2->type;
            if(type2->kind==FUNCTION_KIND){
                type2 = type2->detail.function.returnType;
            }
            if(compareType(type1,type2)==FALSE){
                showError(7,loc1,0,"");
            }
            return symbol1;
        }else if(EQUAL(currentChild,"LB")){//数组
            PrintToken(currentChild,depth);
            char * indexName = currentChild->firstChild->element->val;
            
            symbol2 = Exp(currentChild,depth+1);NEXT(currentChild);
            name2 = symbol2->name;
            loc2 = symbol2->loc;
            type2 = symbol2->type;
            if(type1->kind!=ARRAY_KIND){
                
                showError(10,loc2,1,name1);
            }
            if(type2==NULL||type2->kind!=BASIC_KIND||type2->detail.basicType!=INT_KIND){
                showError(12,loc2,1,indexName);
            }
            PrintToken(currentChild,depth);
            //考虑解引用
            return newSymbol("unpackArray",loc1,symbol1->type->detail.array.elem);

        }else if(EQUAL(currentChild,"DOT")){//结构体
            PrintToken(currentChild,depth);//DOT

            name2 = currentChild->element->val;
            loc2 = currentChild->element->lineNo;
            if(type1==NULL||type1->kind!=STRUCT_KIND){
                showError(13,loc2,0,"");
            }
            if( type1==NULL||searchField(type1->detail.structure.field,name2)==NULL ){
                showError(14,loc2,1,name2);
            }

            PrintToken(currentChild,depth);//ID
            return symbol1;
        }

        
    }else if(EQUAL(currentChild,"LP")){
        PrintToken(currentChild,depth);
        symbol1 = Exp(currentChild,depth+1);NEXT(currentChild);
        PrintToken(currentChild,depth);
        return symbol1;
    }else if(EQUAL(currentChild,"MINUS")){
        PrintToken(currentChild,depth);
        symbol1 = Exp(currentChild,depth+1);NEXT(currentChild);
        return symbol1;
    }else if(EQUAL(currentChild,"NOT")){//BOOL判断
        PrintToken(currentChild,depth);
        return Exp(currentChild,depth+1);
    }else if(EQUAL(currentChild,"ID")){
        if(currentChild->nextSib){//ID(Args)
            symbol1 = searchSymbol(table,currentChild->element->val);
            if(symbol1==NULL){//函数无定义
                showError(2,currentChild->element->lineNo,1,currentChild->element->val);
                return newSymbol(currentChild->element->val,currentChild->element->lineNo,NULL);
            }else if(symbol1->type->kind!=FUNCTION_KIND){//函数有定义，但不是函数
                showError(11,currentChild->element->lineNo,1,currentChild->element->val);
                return newSymbol(currentChild->element->val,currentChild->element->lineNo,NULL);
            }else{
                loc1 = currentChild->element->lineNo;
                PrintToken(currentChild,depth);//ID
                PrintToken(currentChild,depth);//LP
                if(EQUAL(currentChild,"Args")){
                    MultiFieldPointer formalArgs = symbol1->type->detail.function.argv;
                    MultiFieldPointer actualArgs = NULL;
                    actualArgs = Args(currentChild,depth+1);NEXT(currentChild);
                    char* actualArgsString = GetParam(actualArgs);
                    char* formalArgsString = GetParam(formalArgs);
                    if(compareParam(formalArgs,actualArgs)==FALSE){
                        showError(9,loc1,3,actualArgsString,formalArgsString,symbol1->name);  //补全
                    }
                }
                PrintToken(currentChild,depth);
                return symbol1;
            }
            return NULL;
        }else{//ID
            symbol1 = searchSymbol(table,currentChild->element->val);
            if(symbol1==NULL){
                showError(1,currentChild->element->lineNo,1,currentChild->element->val);
                return newSymbol(currentChild->element->val,currentChild->element->lineNo,NULL);
            }
            PrintToken(currentChild,depth);
            return symbol1;
        }
    }else if(EQUAL(currentChild,"INT")){
        symbol1 = newSymbol("int",currentChild->element->lineNo,newType(BASIC_KIND,BASIC_ARGC,INT_KIND));
        PrintToken(currentChild,depth);
        return symbol1;
    }else if(EQUAL(currentChild,"FLOAT")){
        symbol1 = newSymbol("float",currentChild->element->lineNo,newType(BASIC_KIND,BASIC_ARGC,FLOAT_KIND));
        PrintToken(currentChild,depth);
        return symbol1;
    }
}

// Args -> Exp COMMA Args
//       | Exp
MultiFieldPointer Args(NodePointer node, int depth){

    simplePrint(node,depth);

    MultiFieldPointer multiField = NULL;
    SymbolPointer symbol = NULL;
    TypePointer type = NULL;

    NodePointer currentChild = node->firstChild;
    symbol = Exp(currentChild,depth+1);NEXT(currentChild);
    type = symbol->type;
    multiField = newMultiField("",0,type);
    if(currentChild){//Exp COMMA Args
        MultiFieldPointer temp;
        PrintToken(currentChild,depth);//COMMA
        temp = Args(currentChild,depth+1);NEXT(currentChild);
        multiField = mergeMultiField(multiField,temp);
    }
    return multiField;
}

//OptTag -> ID
//      | e
char* OptTag(NodePointer node, int depth){
        if(!node) return "NULL";
        simplePrint(node,depth);
        NodePointer currentChild = node->firstChild;
        if(currentChild){
            char * s = currentChild->element->val;
            PrintToken(currentChild,depth);
            return s;
        }else{
            printf("Unnamed\n");
            return "Unnamed";
        }
}

//Tag->ID
char* Tag(NodePointer node, int depth){
    if(!node) return "NULL";
    simplePrint(node,depth);
    NodePointer currentChild = node->firstChild;
    char * s = currentChild->element->val;
    PrintToken(currentChild,depth);
    return s;
}