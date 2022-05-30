#define NEXT(p) if(p==NULL) printf("FUCK NULL\n"); p=p->nextSib
#define EQUAL(p,type) !strcmp(p->element->name,type)
#define PrintToken(p) {simplePrint(p);NEXT(p);}

#define XXX {printf("Good");exit(0);}

#include "InterCode.h"
#include "stdio.h"
#include "Mysemantic.h"
#include "string.h"

int temp_no_temp = 1;
int temp_no_variable = 1;
int temp_no_label = 1;
// ArgsList Function

ArgsListPointer new_argsList(OperandPointer arg) {
    ArgsListPointer p = (ArgsListPointer)malloc(sizeof(ArgsList));
    p->arg=arg;
    p->prev = p->next = NULL;
    return p;
}

ArgCodetuplePointer new_argcodetuple(ArgsListPointer arglist, InterCodesPointer code){
    ArgCodetuplePointer p = (ArgCodetuplePointer)malloc(sizeof(ArgCodetuple));
    p->arglist = arglist;
    p->code = code;
    return p;
}

ArgsListPointer contactArgsList(ArgsListPointer argslist1, ArgsListPointer argslist2){
    if(argslist1 == NULL) return argslist2;
    if(argslist2 == NULL) return argslist1;
    ArgsListPointer p = argslist1;
    while(p->next != NULL){
        p = p->next;
    }
    p->next = argslist2;
    argslist2->prev = p;
    return argslist1;
}


OperandPointer new_operand(OperandKind kind,...){
    OperandPointer operandPointer = (OperandPointer)malloc(sizeof(Operand));
    operandPointer->kind = kind;
    va_list valist;
    switch(kind){
        case OP_CONSTANT:
            va_start(valist,1);
            operandPointer->detail.value = va_arg(valist,int);
            va_end(valist);
            break;
        case OP_VARIABLE:
        case OP_ADDRESS:
        case OP_TEMP:
        case OP_LABEL:
        case OP_FUNCTION:
        case OP_ARRAY:
        case OP_STRUCTURE:
        case OP_RETURN_ADDRESS:
        case OP_RELOP:
            va_start(valist,1);
            operandPointer->detail.name = va_arg(valist,char*);
            va_end(valist);
            break;
    }
    return operandPointer;
}

OperandPointer new_temp(){
    OperandPointer operandPointer = (OperandPointer)malloc(sizeof(Operand));
    operandPointer->kind = OP_TEMP;
    operandPointer->detail.name = (char*)malloc(sizeof(char)*10);
    sprintf(operandPointer->detail.name,"t%d",temp_no_temp++);
    return operandPointer;
}

OperandPointer new_constant(int value){
    OperandPointer operandPointer = (OperandPointer)malloc(sizeof(Operand));
    operandPointer->kind = OP_CONSTANT;
    operandPointer->detail.value = value;
    return operandPointer;
}

OperandPointer new_label(){
    OperandPointer operandPointer = (OperandPointer)malloc(sizeof(Operand));
    operandPointer->kind = OP_LABEL;
    operandPointer->detail.name = (char*)malloc(sizeof(char)*10);
    sprintf(operandPointer->detail.name,"LABEL%d",temp_no_label++);
    return operandPointer;
}

OperandPointer new_function(char* name){
    OperandPointer operandPointer = (OperandPointer)malloc(sizeof(Operand));
    operandPointer->kind = OP_FUNCTION;
    operandPointer->detail.name = (char*)malloc(sizeof(char)*strlen(name)+1);
    strcpy(operandPointer->detail.name,name);
    return operandPointer;
}

OperandPointer new_variable(char* symbol){
    //printf("%s",symbol);
    OperandListPointer p = variableList;
    while(p){
        if(!strcmp(p->op->symbol,symbol)){
            return p->op;
        }
        p = p->next;
    }
    
    OperandPointer operandPointer = (OperandPointer)malloc(sizeof(Operand));
    operandPointer->kind = OP_VARIABLE;
    operandPointer->detail.name = (char*)malloc(sizeof(char)*10);
    operandPointer->symbol = (char*)malloc(sizeof(char)*10);
    sprintf(operandPointer->detail.name,"v%d",temp_no_variable++);
    
    strcpy(operandPointer->symbol,symbol);
    p = variableList;
    if(p){
        while(p->next)
            p = p->next;
        p->next = (OperandListPointer)malloc(sizeof(OperandList));
        p->next->op = operandPointer;
        p->next->next = NULL;
    }else{
        variableList = (OperandListPointer)malloc(sizeof(OperandList));
        variableList->op = operandPointer;
        variableList->next = NULL;
    }
    
    return operandPointer;
}

InterCodePointer new_intercode(InstructionKind kind,...){
    InterCodePointer interCodePointer = (InterCodePointer)malloc(sizeof(InterCode));
    interCodePointer->kind = kind;
    va_list valist;
    switch(kind){
        case Code_ASSIGN:
            va_start(valist,2);
            interCodePointer->detail.assign.left = va_arg(valist,OperandPointer);
            interCodePointer->detail.assign.right = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_ADD:
        case Code_SUB:
        case Code_MUL:
        case Code_DIV:
            va_start(valist,3);
            interCodePointer->detail.binop.result = va_arg(valist,OperandPointer);
            interCodePointer->detail.binop.op1 = va_arg(valist,OperandPointer);
            interCodePointer->detail.binop.op2 = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_IF:
            va_start(valist,4);
            interCodePointer->detail.if_.op1 = va_arg(valist,OperandPointer);
            interCodePointer->detail.if_.relop = va_arg(valist,OperandPointer);
            interCodePointer->detail.if_.op2 = va_arg(valist,OperandPointer);
            interCodePointer->detail.if_.label = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_LABEL:
            va_start(valist,1);
            interCodePointer->detail.label.op = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_GOTO:
            va_start(valist,1);
            interCodePointer->detail.goto_.op = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_DEC:
            va_start(valist,2);
            interCodePointer->detail.dec.op = va_arg(valist,OperandPointer);
            interCodePointer->detail.dec.size = va_arg(valist,int);
            va_end(valist);
            break;
        case Code_ARG:
            va_start(valist,1);
            interCodePointer->detail.arg.op = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_CALL:
            va_start(valist,2);
            interCodePointer->detail.call.result = va_arg(valist,OperandPointer);
            interCodePointer->detail.call.func = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_PARAM:
            va_start(valist,1);
            interCodePointer->detail.param.op = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_READ:
            va_start(valist,1);
            interCodePointer->detail.read.op = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_WRITE:
            va_start(valist,1);
            interCodePointer->detail.write.op = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_FUNCTION:
            va_start(valist,1);
            interCodePointer->detail.func.op = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
        case Code_RETURN:
            va_start(valist,1);
            interCodePointer->detail.return_.op = va_arg(valist,OperandPointer);
            va_end(valist);
            break;
    }
    return interCodePointer;
}

//InterCodes Function

InterCodesPointer new_intercodes(InterCodePointer code){
    InterCodesPointer interCodesPointer = (InterCodesPointer)malloc(sizeof(InterCodes));
    interCodesPointer->code = code;
    interCodesPointer->prev = NULL;
    interCodesPointer->next = NULL;
    return interCodesPointer;
}

InterCodesPointer contactInterCodes(InterCodesPointer p1,InterCodesPointer p2){
    if(p1==NULL) return p2;
    if(p2==NULL) return p1;
    InterCodesPointer p = p1;
    while(p->next!=NULL){
        p=p->next;
    }
    p->next = p2;
    p2->prev = p;
    return p1;
}



// Gneral Function##############################################################

void print_code(InterCodesPointer codes){
    while(codes!=NULL){
        switch(codes->code->kind){
            case Code_ASSIGN:
                print_op(codes->code->detail.assign.left);
                printf(" := ");
                print_op(codes->code->detail.assign.right);
                printf("\n");
                break;
            case Code_ADD:
                print_op(codes->code->detail.binop.result);
                printf(" := ");
                print_op(codes->code->detail.binop.op1);
                printf(" + ");
                print_op(codes->code->detail.binop.op2);
                printf("\n");
                break;
            case Code_SUB:
                print_op(codes->code->detail.binop.result);
                printf(" := ");
                print_op(codes->code->detail.binop.op1);
                printf(" - ");
                print_op(codes->code->detail.binop.op2);
                printf("\n");
                break;
            case Code_MUL:
                print_op(codes->code->detail.binop.result);
                printf(" := ");
                print_op(codes->code->detail.binop.op1);
                printf(" * ");
                print_op(codes->code->detail.binop.op2);
                printf("\n");
                break;
            case Code_DIV:
                print_op(codes->code->detail.binop.result);
                printf(" := ");
                print_op(codes->code->detail.binop.op1);
                printf(" / ");
                print_op(codes->code->detail.binop.op2);
                printf("\n");
                break;
            case Code_LABEL:
                printf("LABEL ");
                print_op(codes->code->detail.label.op);
                printf(" :\n");
                break;
            case Code_GOTO:
                printf("GOTO ");
                print_op(codes->code->detail.goto_.op);
                printf("\n");
                break;
            case Code_IF://TODO
                printf("IF ");
                print_op(codes->code->detail.if_.op1);
                printf(" ");
                print_op(codes->code->detail.if_.relop);
                printf(" ");
                print_op(codes->code->detail.if_.op2);
                printf(" GOTO ");
                print_op(codes->code->detail.if_.label);
                printf("\n");
                break;
            case Code_DEC:
                printf("DEC ");
                print_op(codes->code->detail.dec.op);
                printf(" %d\n",codes->code->detail.dec.size);
                break;
            case Code_ARG:
                printf("ARG ");
                print_op(codes->code->detail.arg.op);
                printf("\n");
                break;
            case Code_CALL:
                print_op(codes->code->detail.call.result);
                printf(" := CALL ");
                print_op(codes->code->detail.call.func);
                printf("\n");
                break;
            case Code_PARAM:
                printf("PARAM ");
                print_op(codes->code->detail.param.op);
                printf("\n");
                break;
            case Code_READ:
                printf("READ ");
                print_op(codes->code->detail.read.op);
                printf("\n");
                break;
            case Code_WRITE:
                printf("WRITE ");
                print_op(codes->code->detail.write.op);
                printf("\n");
                break;
            case Code_FUNCTION:
                printf("FUNCTION ");
                print_op(codes->code->detail.func.op);
                printf(" :\n");
                break;
            case Code_RETURN:
                printf("RETURN ");
                print_op(codes->code->detail.return_.op);
                printf("\n");
                break;
        }
        
        codes = codes->next;
    }
}

void print_op(OperandPointer op){
    if(op->kind==OP_CONSTANT){
        printf("#%d",op->detail.value);
    }else{
        printf("%s",op->detail.name);
    }
}


void traverseTreeForTranslate(NodePointer node){
    if(!node)return;
    if(EQUAL(node,"ExtDef")){
        translate_ExtDef(node);
    }
    traverseTreeForTranslate(node->firstChild);
    traverseTreeForTranslate(node->nextSib);
}



// ExtDef -> Specifier ExtDecList SEMI
//         | Specifier SEMI
//         | Specifier FunDec CompSt
void translate_ExtDef(NodePointer node){
    if(verbose){
        printf("ExtDef,%d\n",node->element->lineNo);
    }
    NodePointer currentChild=node->firstChild;
    translate_Specifier(currentChild);NEXT(currentChild);
    if(EQUAL(currentChild,"ExtDecList")){//Specifier ExtDecList SEMI 全局变量 OK
        translate_ExtDecList(currentChild);NEXT(currentChild);//ExtDecList
        NEXT(currentChild);//SEMI
    }else if(EQUAL(currentChild,"SEMI")){//Specifier SEMI
        NEXT(currentChild);//SEMI

    }else if(EQUAL(currentChild,"FunDec")){//Specifier FunDec CompSt

        translate_FunDec(currentChild);NEXT(currentChild);//FunDec
        translate_CompSt(currentChild);NEXT(currentChild);
    }
}

// ExtDecList -> VarDec
//             | VarDec COMMA ExtDecList
void translate_ExtDecList(NodePointer node){
    if(verbose)
    printf("ExtDecList,%d\n",node->element->lineNo);
    NodePointer currentChild=node->firstChild;
    translate_VarDec(currentChild);NEXT(currentChild);
    if(currentChild){//VarDec COMMA ExtDecList
        NEXT(currentChild);
        translate_ExtDecList(currentChild);
    }else{//VarDec
        
    }
}

// Specifier -> TYPE
//            | StructSpecifier
void translate_Specifier(NodePointer node){
    if(verbose)
    printf("Specifier,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;
    if(EQUAL(currentChild,"TYPE")){

    }else if(EQUAL(currentChild,"StructSpecifier")){
        translate_StructSpecifier(currentChild);NEXT(currentChild);//Todo
    }
}

// StructSpecifier -> STRUCT OptTag LC DefList RC
//                  | STRUCT Tag
void translate_StructSpecifier(NodePointer node){
    if(verbose)
    printf("StructSpecifier,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;


    NEXT(currentChild); //Struct

    if(EQUAL(currentChild,"OptTag")||EQUAL(currentChild,"LC")){
        if(EQUAL(currentChild,"OptTag")){
            translate_OptTag(currentChild);NEXT(currentChild);//OptTag
        }
        NEXT(currentChild);//LC
        
        if(EQUAL(currentChild,"DefList")){
            translate_DefList(currentChild);NEXT(currentChild);//DefList
        }

        NEXT(currentChild);//RC

    }else if(EQUAL(currentChild,"Tag")){//Tag …… 不能不管
        translate_Tag(currentChild);NEXT(currentChild);
    }
}

// VarDec -> ID
//         | VarDec LB INT RB
void translate_VarDec(NodePointer node){
    if(verbose)
    printf("VarDec,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;
    if(EQUAL(currentChild,"ID")){
        return;
    }else if(EQUAL(currentChild,"VarDec")){

        translate_VarDec(currentChild);NEXT(currentChild);

    }
    
}

// FunDec -> ID LP VarList RP
//         | ID LP RP
InterCodesPointer translate_FunDec(NodePointer node){
    if(verbose)
    printf("FunDec,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;

    SymbolPointer func = searchSymbol(table,currentChild->element->val);

    InterCodesPointer interCodes = new_intercodes(new_intercode(Code_FUNCTION,new_operand(OP_FUNCTION,func->name)));

    MultiFieldPointer argv = func->type->detail.function.argv;
    
    while(argv){
        if(argv->type->detail.basicType == INT_KIND){    
            interCodes = contactInterCodes(interCodes, new_intercodes(new_intercode(Code_PARAM, new_variable(argv->name)))); 
          }
        argv = argv->nextField;
    }

    NEXT(currentChild); //ID
    NEXT(currentChild); //LP
    if(EQUAL(currentChild,"VarList")){
        translate_VarList(currentChild);NEXT(currentChild);
        
    }else if(EQUAL(currentChild,"RP")){
    }
    codeRoot = contactInterCodes(codeRoot,interCodes);
    return interCodes;
}

// VarList -> ParamDec COMMA VarList
//          | ParamDec
void translate_VarList(NodePointer node){
    if(verbose)
    printf("VarList\n");
    NodePointer currentChild = node->firstChild;

    translate_ParamDec(currentChild);NEXT(currentChild);

    if(currentChild){//VarList -> ParamDec COMMA VarList
        NEXT(currentChild); //COMMA
        translate_VarList(currentChild);
    }
}

// ParamDec -> Specifier VarDec
void translate_ParamDec(NodePointer node){
    if(verbose)
    printf("ParamDec,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;

    translate_Specifier(currentChild);NEXT(currentChild);
    translate_VarDec(currentChild);NEXT(currentChild);
}

// CompSt -> LC DefList StmtList RC
InterCodesPointer translate_CompSt(NodePointer node){
    if(verbose)
    printf("CompSt,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;
    
    NEXT(currentChild);
    if(EQUAL(currentChild,"DefList")){
        translate_DefList(currentChild);
        NEXT(currentChild); 
    }
    
    if(EQUAL(currentChild,"StmtList")){
        translate_StmtList(currentChild);NEXT(currentChild);
    }
    NEXT(currentChild);
}

// StmtList -> Stmt StmtList
//           | e
void translate_StmtList(NodePointer node){

    if(!node) return;

    if(verbose)
    printf("StmtList,%d\n",node->element->lineNo);

    NodePointer currentChild = node->firstChild;

    
    codeRoot = contactInterCodes(codeRoot,translate_Stmt(currentChild));
    NEXT(currentChild);
    translate_StmtList(currentChild);
    
}




// Stmt -> Exp SEMI
//       | CompSt
//       | RETURN Exp SEMI
//       | IF LP Exp RP Stmt
//       | IF LP Exp RP Stmt ELSE Stmt
//       | WHILE LP Exp RP Stmt
InterCodesPointer translate_Stmt(NodePointer node){
    if(verbose)
    printf("Stmt,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;

    if(EQUAL(currentChild,"Exp")){ //Exp SEMI
        return translate_Exp(currentChild,NULL);
    }else if(EQUAL(currentChild,"CompSt")){//CompSt
        return translate_CompSt(currentChild);
        
    }else if(EQUAL(currentChild,"RETURN")){
        NEXT(currentChild);
        OperandPointer t1 = new_temp();
        InterCodesPointer interCodes1 = translate_Exp(currentChild,&t1);
        InterCodesPointer interCodes2 = new_intercodes(new_intercode(Code_RETURN,t1));
        InterCodesPointer InterCodes = contactInterCodes(interCodes1,interCodes2);
        return InterCodes;
    }else if(EQUAL(currentChild,"IF")){
        NEXT(currentChild);//Code_IF
        NEXT(currentChild);//LP
        OperandPointer label1 = new_label();
        OperandPointer label2 = new_label();
        InterCodesPointer interCodes1 = translate_Cond(currentChild,label1,label2);NEXT(currentChild);//Exp
        NEXT(currentChild);//RP
        InterCodesPointer interCodes2 = translate_Stmt(currentChild);NEXT(currentChild);//Stmt
        InterCodesPointer interCodes;
        
        if(!currentChild){//Code_IF LP Exp RP Stmt
            interCodes = contactInterCodes(interCodes1,contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label1)) ,contactInterCodes(interCodes2,new_intercodes(new_intercode(Code_LABEL,label2)))));
        }else{//Code_IF LP Exp RP Stmt ELSE Stmt
            NEXT(currentChild);//ELSE
            OperandPointer label3 = new_label();
            InterCodesPointer interCodes3 = translate_Stmt(currentChild);
            interCodes = contactInterCodes(interCodes1,contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label1)) ,contactInterCodes(interCodes2,contactInterCodes(new_intercodes(new_intercode(Code_GOTO,label3)),contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label2)),contactInterCodes(interCodes3,new_intercodes(new_intercode(Code_LABEL,label3))))))));
        }
        return interCodes;
    }else if(EQUAL(currentChild,"WHILE")){
        NEXT(currentChild);//WHILE
        NEXT(currentChild);//LP
        OperandPointer label1 = new_label();
        OperandPointer label2 = new_label();
        OperandPointer label3 = new_label();
        InterCodesPointer interCodes1 = translate_Cond(currentChild,label2,label3);NEXT(currentChild);//Exp
        NEXT(currentChild);//RP
        InterCodesPointer interCodes2 = translate_Stmt(currentChild);
        InterCodesPointer interCodes = contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label1)),contactInterCodes(interCodes1,contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label2)),contactInterCodes(interCodes2,contactInterCodes(new_intercodes(new_intercode(Code_GOTO,label1)),new_intercodes(new_intercode(Code_LABEL,label3)))))));
        
        return interCodes;
    }
}

// DefList -> Def DefList
//          | e
void translate_DefList(NodePointer node){
    if(verbose)
    printf("DefList,%d\n",node->element->lineNo);
    if(node == NULL) return;

    NodePointer currentChild = node->firstChild;

    translate_Def(currentChild);NEXT(currentChild);

    if(currentChild){
        translate_DefList(currentChild);NEXT(currentChild);
    }

}

// Def -> Specifier DecList SEMI
void translate_Def(NodePointer node){
    if(verbose)
    printf("Def,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;

    translate_Specifier(currentChild);NEXT(currentChild);
    translate_DecList(currentChild);NEXT(currentChild);
    NEXT(currentChild);
}

// DecList -> Dec
//          | Dec COMMA DecList
void translate_DecList(NodePointer node){
    if(verbose)
    printf("DecList,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;
    
    translate_Dec(currentChild);NEXT(currentChild);
    
    if(currentChild){
        NEXT(currentChild); //COMMA
        translate_DecList(currentChild);NEXT(currentChild);
    }
    
}

// Dec -> VarDec
//      | VarDec ASSIGNOP Exp?
void translate_Dec(NodePointer node){
    if(verbose)
    printf("Dec,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;
    
    translate_VarDec(currentChild);NEXT(currentChild);
    
    if(currentChild){//VarDec ASSIGNOP 
        NEXT(currentChild);//ASSIGNOP
        translate_Exp(currentChild,NULL);NEXT(currentChild);
           //To Do 研究错误 一般不考量这个问题 问就是偷懒
    }
}

// Exp -> Exp ASSIGNOP Exp
//      | Exp AND Exp
//      | Exp OR Exp
//      | Exp RELOP Exp
//      | Exp PLUS Exp
//      | Exp MINUS Exp
//      | Exp STAR Exp
//      | Exp Code_DIV Exp
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
InterCodesPointer translate_Exp(NodePointer node, OperandPointer* operandPointer){
    OperandPointer operand = NULL;
    if(operandPointer)
        operand= *operandPointer; 

    if(verbose)
        printf("Exp,%d\n",node->element->lineNo);
    
    NodePointer currentChild = node->firstChild;
    
    SymbolPointer symbol1;
    SymbolPointer symbol2;


    if(EQUAL(currentChild,"Exp")){
        NodePointer node1 = currentChild;
        
        symbol1 = searchSymbol(table,currentChild->firstChild->element->val);
        NEXT(currentChild);
        if(EQUAL(currentChild,"ASSIGNOP")){
            
            //symbol2 = searchSymbol(table,currentChild->element->val);
            NEXT(currentChild);
            OperandPointer p = new_variable(symbol1->name);
            InterCodesPointer interCodes;

            if(EQUAL(currentChild->firstChild,"ID")&&currentChild->firstChild->nextSib){
                interCodes = translate_Exp(currentChild,&p);
            }else{
                OperandPointer temp = new_temp();
                InterCodesPointer interCodes1 = translate_Exp(currentChild,&temp);
                InterCodesPointer interCodes2 = operand? contactInterCodes(new_intercodes(new_intercode(Code_ASSIGN,p,temp)),new_intercodes(new_intercode(Code_ASSIGN,operand,p))):new_intercodes(new_intercode(Code_ASSIGN,p,temp));
                interCodes = contactInterCodes(interCodes1,interCodes2);
            }
            
            
            return interCodes;
        }else if(EQUAL(currentChild,"AND")){
            OperandPointer label1 = new_label();
            OperandPointer label2 = new_label();
            InterCodesPointer interCodes0 = new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(0)));
            InterCodesPointer interCodes1 = translate_Cond(node,label1,label2);
            InterCodesPointer interCodes2 = contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label1)),new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(1))));
            InterCodesPointer interCodes = contactInterCodes(interCodes0,contactInterCodes(interCodes1,contactInterCodes(interCodes2,new_intercodes(new_intercode(Code_LABEL,label2)))));
            
            return interCodes;

        }else if(EQUAL(currentChild,"OR")){
            OperandPointer label1 = new_label();
            OperandPointer label2 = new_label();
            InterCodesPointer interCodes0 = new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(0)));
            InterCodesPointer interCodes1 = translate_Cond(node,label1,label2);
            InterCodesPointer interCodes2 = contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label1)),new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(1))));
            InterCodesPointer interCodes = contactInterCodes(interCodes0,contactInterCodes(interCodes1,contactInterCodes(interCodes2,new_intercodes(new_intercode(Code_LABEL,label2)))));
            
            return interCodes;

        }else if(EQUAL(currentChild,"RELOP")){
            NEXT(currentChild);
            symbol2 = searchSymbol(table,currentChild->element->val);

            OperandPointer label1 = new_label();
            OperandPointer label2 = new_label();
            InterCodesPointer interCodes0 = new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(0)));
            InterCodesPointer interCodes1 = translate_Cond(node,label1,label2);
            InterCodesPointer interCodes2 = contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label1)),new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(1))));
            InterCodesPointer interCodes = contactInterCodes(interCodes0,contactInterCodes(interCodes1,contactInterCodes(interCodes2,new_intercodes(new_intercode(Code_LABEL,label2)))));
            
            
            
            return interCodes;
        }else if(EQUAL(currentChild,"PLUS")){
            OperandPointer t1 = new_temp();
            OperandPointer t2 = new_temp();
            InterCodesPointer interCodes1 = translate_Exp(node1,&t1);
            NEXT(currentChild);
            InterCodesPointer interCodes2 = translate_Exp(currentChild,&t2);
            InterCodesPointer interCodes3 = new_intercodes(new_intercode(Code_ADD,operand,t1,t2));
            InterCodesPointer interCodes = contactInterCodes(interCodes1,contactInterCodes(interCodes2,interCodes3));
            
            return interCodes;
        }else if(EQUAL(currentChild,"MINUS")){
            OperandPointer t1 = new_temp();
            OperandPointer t2 = new_temp();
            InterCodesPointer interCodes1 = translate_Exp(node1,&t1);
            NEXT(currentChild);
            InterCodesPointer interCodes2 = translate_Exp(currentChild,&t2);
            InterCodesPointer interCodes3 = new_intercodes(new_intercode(Code_SUB,operand,t1,t2));
            InterCodesPointer interCodes = contactInterCodes(interCodes1,contactInterCodes(interCodes2,interCodes3));
            
            return interCodes;

        }else if(EQUAL(currentChild,"STAR")){
            OperandPointer t1 = new_temp();
            InterCodesPointer interCodes1 = translate_Exp(node1,&t1);
            NEXT(currentChild);
            OperandPointer t2 = new_temp();
            InterCodesPointer interCodes2 = translate_Exp(currentChild,&t2);
            InterCodesPointer interCodes3 = new_intercodes(new_intercode(Code_MUL,operand,t1,t2));
            InterCodesPointer interCodes = contactInterCodes(interCodes1,contactInterCodes(interCodes2,interCodes3));
            
            return interCodes;

        }else if(EQUAL(currentChild,"Code_DIV")){
            OperandPointer t1 = new_temp();
            OperandPointer t2 = new_temp();
            InterCodesPointer interCodes1 = translate_Exp(node1,&t1);
            NEXT(currentChild);
            InterCodesPointer interCodes2 = translate_Exp(currentChild,&t2);
            InterCodesPointer interCodes3 = new_intercodes(new_intercode(Code_DIV,operand,t1,t2));
            InterCodesPointer interCodes = contactInterCodes(interCodes1,contactInterCodes(interCodes2,interCodes3));
            
            return interCodes;

        }else if(EQUAL(currentChild,"LB")){//数组


        }else if(EQUAL(currentChild,"DOT")){//结构体

        }

        
    }else if(EQUAL(currentChild,"LP")){
        NEXT(currentChild);
        InterCodesPointer interCodes = translate_Exp(currentChild,operandPointer);
        NEXT(currentChild);
        return interCodes;
    }else if(EQUAL(currentChild,"MINUS")){
        OperandPointer t1 = new_temp();
        InterCodesPointer interCodes1 = translate_Exp(currentChild,&t1);
        InterCodesPointer interCodes2 = new_intercodes(new_intercode(Code_SUB,operand,new_constant(0),t1));
        InterCodesPointer interCodes = contactInterCodes(interCodes1,interCodes2);
        
        return interCodes;
        
    }else if(EQUAL(currentChild,"NOT")){//BOOL判断
        OperandPointer label1 = new_label();
        OperandPointer label2 = new_label();
        InterCodesPointer interCodes0 = new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(0)));
        InterCodesPointer interCodes1 = translate_Cond(node,label1,label2);
        InterCodesPointer interCodes2 = contactInterCodes(new_intercodes(new_intercode(Code_LABEL,label1)),new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(1))));
        InterCodesPointer interCodes = contactInterCodes(interCodes0,contactInterCodes(interCodes1,contactInterCodes(interCodes2,new_intercodes(new_intercode(Code_LABEL,label2)))));
        
        return interCodes;

    }else if(EQUAL(currentChild,"ID")){
        if(currentChild->nextSib){//ID(Args)
            SymbolPointer function = searchSymbol(table,currentChild->element->val);
            NEXT(currentChild);//ID
            NEXT(currentChild);//LP
            if(EQUAL(currentChild,"Args")){
                ArgsListPointer argsList = NULL;
                ArgCodetuplePointer res = translate_Args(currentChild,argsList);
                InterCodesPointer interCodes1 = res->code;
                argsList = res ->arglist;
                if(!strcmp(function->name,"write")){
                    InterCodesPointer interCodes = contactInterCodes(interCodes1,new_intercodes(new_intercode(Code_WRITE,argsList->arg)));
                    return interCodes;
                }else{
                    InterCodesPointer interCodes2 = NULL;
                    ArgsListPointer p = argsList;
                    while(p){
                        interCodes2 = contactInterCodes(interCodes2,new_intercodes(new_intercode(Code_ARG,p->arg)));
                        p = p->next;
                    }
                    InterCodesPointer interCodes = contactInterCodes(interCodes1,interCodes2);
                    interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_CALL,operand,new_function(function->name))));
                    
                    return interCodes;
                }
                
            }else{
                if(!strcmp(function->name,"read")){
                    InterCodesPointer interCodes = new_intercodes(new_intercode(Code_READ,operand)); 
                    //codeRoot = contactInterCodes(codeRoot,interCodes);
                    return interCodes;
                }else{
                    InterCodesPointer interCodes = new_intercodes(new_intercode(Code_CALL,operand,new_function(function->name))); 
                    return interCodes;
                }
            }

        }else{//ID
            SymbolPointer symbol = searchSymbol(table,currentChild->element->val);
            int val = atoi(currentChild->element->val);
            if((*operandPointer)->kind == OP_TEMP)
                temp_no_temp--;
            *operandPointer = new_variable(symbol->name);
            InterCodesPointer interCodes = NULL;
            //InterCodesPointer interCodes = new_intercodes(new_intercode(Code_ASSIGN,operand,new_variable(symbol->name)));
            
            return interCodes;
        }
    }else if(EQUAL(currentChild,"INT")){
        
        int val = atoi(currentChild->element->val);
        if((*operandPointer)->kind == OP_TEMP)
            temp_no_temp--;
        *operandPointer = new_constant(val);
        InterCodesPointer interCodes = NULL;
        //InterCodesPointer interCodes = operand?new_intercodes(new_intercode(Code_ASSIGN,operand,new_constant(val))):NULL;
        
        return interCodes;
    }else if(EQUAL(currentChild,"FLOAT")){
        return NULL;//不会出现
    }
}




// Cond -> Exp RELOP Exp
//       | NOT Exp
//       | Exp AND Exp
//       | Exp OR Exp
//       | others
InterCodesPointer translate_Cond(NodePointer node, OperandPointer label_true, OperandPointer label_false){
    if(verbose)
    printf("translate_Cond,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;
    if(EQUAL(currentChild,"Exp")&&EQUAL(currentChild->nextSib,"RELOP")){
        OperandPointer t1 = new_temp();
        InterCodesPointer interCodes1 = translate_Exp(currentChild,&t1);
        NEXT(currentChild);//Exp
        OperandPointer op = new_operand(OP_RELOP,currentChild->element->val);
        NEXT(currentChild);
        OperandPointer t2 = new_temp();
        InterCodesPointer interCodes2 = translate_Exp(currentChild,&t2);
        InterCodesPointer interCodes = contactInterCodes(interCodes1,interCodes2);
        interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_IF,t1,op,t2,label_true)));
        interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_GOTO,label_false)));
        
        return interCodes;
    }else if(EQUAL(currentChild,"Exp")&&EQUAL(currentChild->nextSib,"AND")){
        OperandPointer t1 = new_temp();
        InterCodesPointer interCodes1 = translate_Exp(currentChild,&t1);
        NEXT(currentChild);
        OperandPointer op = new_operand(OP_RELOP,currentChild->element->val);
        NEXT(currentChild);
        OperandPointer t2 = new_temp();
        InterCodesPointer interCodes2 = translate_Exp(currentChild,&t2);
        InterCodesPointer interCodes = contactInterCodes(interCodes1,interCodes2);
        interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_IF,op,t1,label_true)));
        interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_GOTO,label_false)));
       
        return interCodes;
    }else if(EQUAL(currentChild,"Exp")&&EQUAL(currentChild->nextSib,"OR")){
        OperandPointer t1 = new_temp();
        InterCodesPointer interCodes1 = translate_Exp(currentChild,&t1);
        NEXT(currentChild);
        OperandPointer op = new_operand(OP_RELOP,currentChild->element->val);
        NEXT(currentChild);
        OperandPointer t2 = new_temp();
        InterCodesPointer interCodes2 = translate_Exp(currentChild,&t2);
        InterCodesPointer interCodes = contactInterCodes(interCodes1,interCodes2);
        interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_IF,op,t1,label_true)));
        interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_GOTO,label_false)));
        
        return interCodes;
    }else if(EQUAL(currentChild,"NOT")){
        NEXT(currentChild);
        OperandPointer label1 = new_label();
        OperandPointer label2 = new_label();
        InterCodesPointer interCodes1 = translate_Cond(currentChild,label1,label2);
        InterCodesPointer interCodes = contactInterCodes(interCodes1,new_intercodes(new_intercode(Code_LABEL,label1)));
        interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_GOTO,label_true)));
        interCodes = contactInterCodes(interCodes,new_intercodes(new_intercode(Code_LABEL,label2)));
        
        return interCodes;
    }else{
        OperandPointer t1 = new_temp();
        InterCodesPointer interCodes1 = translate_Exp(currentChild,&t1);
        InterCodesPointer interCodes = new_intercodes(new_intercode(Code_IF,t1,label_true));
        
        return interCodes;
    }

}

// Args -> Exp COMMA Args
//       | Exp
//需要串接Arglist
ArgCodetuplePointer translate_Args(NodePointer node, ArgsListPointer argsList){
    if(verbose)
    printf("translate_Args,%d\n",node->element->lineNo);
    NodePointer currentChild = node->firstChild;
    OperandPointer t1 = new_temp();
    InterCodesPointer interCodes1 = translate_Exp(currentChild,&t1);

    argsList = contactArgsList(new_argsList(t1),argsList);

    InterCodesPointer interCodes = interCodes1;

    NEXT(currentChild);
    if(currentChild){
        NEXT(currentChild);//COMMA
        ArgCodetuplePointer argCodetuple2 = translate_Args(currentChild,argsList);
        argsList = argCodetuple2->arglist;
        InterCodesPointer interCodes2 = argCodetuple2->code;
        interCodes = contactInterCodes(interCodes,interCodes2);
    }

    ArgCodetuplePointer argCodetuple = new_argcodetuple(argsList,interCodes);
    return argCodetuple;
}

//OptTag -> ID
//      | e
void translate_OptTag(NodePointer node){
    if(verbose)
    printf("translate_OptTag,%d\n",node->element->lineNo);
    return;
}

//Tag->ID
void translate_Tag(NodePointer node){
    if(verbose)
    printf("translate_Tag,%d\n",node->element->lineNo);
    return;
}