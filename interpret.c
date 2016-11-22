/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

/*--------------------------------LIBRARIES---------------------------------*/


/**
 returny su 99 ak su chyba
**/
// System libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>

// Local libraries
#include "error.h"
#include "ial.h"
#include "ir.h"
#include "int_memory_management.h"
#include "int_debug.h"

#include "interpret.h"

#define TRUE 0
#define FALSE 1

static SymbolTable *symGlob = NULL;
static Stack *GlobalStack = NULL;

static bool continueFlag = FALSE;
static bool breakFlag = FALSE;


#define FN(i, ret, fnName, count, arg) do {                             \
        char *name = strdup_("ifj16." #fnName);                         \
        Function *f = createFunction(name, ret, count, arg);            \
        f->builtin = true;                                              \
        table_insert_function(&i->symTable, f);                         \
    } while (0);

Interpret *createInterpret(void) {
    Interpret *i = malloc(sizeof(Interpret));
    CHECK_ALLOC(i);
    i->symTable.root = NULL;

    Declaration *printArg = createDeclaration(T_STRING, strdup_("x"));
    Declaration *sortArg = createDeclaration(T_STRING, strdup_("x"));
    Declaration *lengthArg = createDeclaration(T_STRING, strdup_("x"));

    Declaration *findArg1 = createDeclaration(T_STRING, strdup_("x"));
    Declaration *findArg2 = createDeclaration(T_STRING, strdup_("y"));
    findArg2->next = findArg1;
    Declaration *compareArg1 = createDeclaration(T_STRING, strdup_("x"));
    Declaration *compareArg2 = createDeclaration(T_STRING, strdup_("y"));
    compareArg2->next = compareArg1;
    Declaration *substrArg1 = createDeclaration(T_STRING, strdup_("x"));
    Declaration *substrArg2 = createDeclaration(T_INTEGER, strdup_("n"));
    Declaration *substrArg3 = createDeclaration(T_INTEGER, strdup_("l"));
    substrArg2->next = substrArg1;
    substrArg3->next = substrArg2;

    table_insert(&i->symTable, createClassNode(strdup_("ifj16")));

    FN(i, T_INTEGER, readInt, 0, NULL);
    FN(i, T_DOUBLE, readDouble, 0, NULL);
    FN(i, T_STRING, readString, 0, NULL);
    FN(i, T_VOID, print, 1, printArg);
    FN(i, T_STRING, sort, 2, sortArg);
    FN(i, T_INTEGER, length, 2, lengthArg);
    FN(i, T_INTEGER, find, 2, findArg2);
    FN(i, T_INTEGER, compare, 2, compareArg2);
    FN(i, T_STRING, substr, 3, substrArg3);

    return i;
}


int freeInterpret(Interpret *i) {
    if (i == NULL)
        return 1;
    if (i->symTable.root != NULL)
        freeNode(i->symTable.root);
    free(i);
    return 0;
}


int evalMain(Interpret *i) {
    assert(i != NULL);
    symGlob = &(i->symTable);
    GlobalStack = createLocalStack(NULL);

    Node *mainFn = table_lookup(&i->symTable, "Main.run");

    assert(mainFn != NULL);

    if (mainFn->type == N_FUNCTION) {
        interpretFunc(GlobalStack, mainFn);
    } else {
        dPrintf("%s %d","Unexpected happened with node!",mainFn->type);
    }

    free(GlobalStack);
    return 0;
}

int interpretFunc(Stack *stack, Node *node){

    // if is node
    Stack *localStack = NULL;
    SymbolTable *localTable = NULL;

    Node *mainFn = table_lookup(symGlob, "Main.run");

    if(strcmp(node->symbol, mainFn->symbol)){
        dPrintf("%s","Creating new local stack.");
        localStack = createLocalStack(stack);
        dPrintf("%s","Creating new local table.");
        localTable = createSymbolTable();
        ht_insert (&alloc_tab, localTable);
    }
    else{
        dPrintf("%s\n\t- %s\n\t- %s","Setting reference of:","local stack to global stack,","local table to global table.");
        localStack = GlobalStack;
        localTable = symGlob;
    }

    assert(localStack != NULL);
    assert(localTable != NULL);

    int fn_type = builtInFunc(localTable, localStack, node->data.function);

    // if fn type was built-in function return
    if(fn_type == 0)
        return 0;

    Command *current = node->data.function->body.head;
    Command *tail = node->data.function->body.tail;

    while(current != tail){
        evalCommand(localTable, localStack, current);
        current = current->next;
    }
    evalCommand(localTable, localStack, current);



    return 0;
}

Value *evalCommand(SymbolTable *symTable, Stack *stack, Command *cmd){
    (void) symTable;
    (void) stack;
    Node *node;
    Value *val;

    Command *current, *tail;

    continueFlag = 0;

    switch(cmd->type){
        case(C_DECLARE):
            //  insert declaration into table
            table_insert_dummy(symTable, cmd->data.declare);
            break;

        case(C_DEFINE):

            // insert dec into table
            table_insert_dummy(symTable, cmd->data.define.declaration);

            // find node and evaluate expression
            node = table_lookup(symTable, cmd->data.define.declaration.name);

            val = evalExpression(cmd->data.define.expr);

            if(val == NULL || node == NULL)
                PERROR("Interpret: CMD: Define: node or value not found.");

            // assign value
            node->data.value = val;

            break;

        case(C_ASSIGN):

            // find node in symTable and evaluate expr
            node = table_lookup(symTable, cmd->data.assign.name);

            if(node == NULL){
                node = table_lookup(symGlob, cmd->data.assign.name);
            }

            if(node == NULL)
                PERROR("Interpret: CMD: Assign: Variable not found in local or global symbol table.");

            val = evalExpression(cmd->data.assign.expr);

            if(val == NULL)
                PERROR("Interpret: CMD: Assign: Evaluation of value was not successful.")

            // assign
            node->data.value = val;

            break;

        case(C_BLOCK):

            evalBlock(symTable, stack, &(cmd->data.block));

            break;

        case(C_IF):
            val = evalExpression(cmd->data.ifC.cond);
            if(val == NULL)
                PERROR("Interpret: CMD: ifC: Evaluation of condition was not successful.")

            if(valueIsZero(val)){

                evalBlock(symTable, stack, &(cmd->data.ifC.thenBlock));
            }
            else{

                evalBlock(symTable, stack, &(cmd->data.ifC.elseBlock));

            }

            break;

        case(C_WHILE):
            val = evalExpression(cmd->data.whileC.cond);
            if(val == NULL)
                PERROR("Interpret: CMD: whileC: Evaluation of condition was not successful.")

            current = cmd->data.whileC.bodyBlock.head;
            tail = cmd->data.whileC.bodyBlock.tail;

            continueFlag = FALSE;

            while(valueIsZero(val)){

                while(current != tail){

                    if(continueFlag)
                        continueFlag = FALSE;

                    evalCommand(symTable, stack, current);

                    if(continueFlag == TRUE){
                        current = cmd->data.whileC.bodyBlock.head;
                        break;
                    }else if(breakFlag == TRUE){
                        break;
                    }
                    else{
                        current = current->next;
                        if(current == tail){
                            evalCommand(symTable, stack, current);
                        }
                    }
                }

                if(breakFlag == TRUE){
                        break;
                }

                val = evalExpression(cmd->data.ifC.cond);

            }

            break;

        case(C_EXPRESSION):
            val = evalExpression(cmd->data.expr);
            break;

        case(C_RETURN):
            ;
            break;

        case(C_FOR):
            ;
            break;

        case(C_CONTINUE):
            continueFlag = TRUE;
            break;

        case(C_BREAK):
            breakFlag = TRUE;
            break;

        case(C_DO_WHILE):
            val = evalExpression(cmd->data.whileC.cond);
            if(val == NULL)
                PERROR("Interpret: CMD: doWhile: Evaluation of condition was not successful.")

            current = cmd->data.whileC.bodyBlock.head;
            tail = cmd->data.whileC.bodyBlock.tail;

            continueFlag = FALSE;

            do{

                while(current != tail){

                    if(continueFlag)
                        continueFlag = FALSE;

                    evalCommand(symTable, stack, current);

                    if(continueFlag == TRUE){
                        current = cmd->data.whileC.bodyBlock.head;
                        break;
                    }
                    else if(breakFlag == TRUE){
                        break;
                    }
                    else{
                        current = current->next;
                        if(current == tail){
                            evalCommand(symTable, stack, current);
                        }
                    }
                }

                if(breakFlag == TRUE){
                        break;
                }

                val = evalExpression(cmd->data.ifC.cond);

            }while(valueIsZero(val));

            break;
    }

    return 0;
}

int evalBlock(SymbolTable *symTable, Stack *stack, Block *block){
    Command *current = block->head;
    Command *tail = block->tail;

    while(current != tail){
        evalCommand(symTable, stack, current);
        current = current->next;
    }

    evalCommand(symTable, stack, current);

    return 0;
}

//  Look for builtin functions
//
int builtInFunc(SymbolTable *symTable, Stack *stack, Function *fn){

    char *str = fn->name;

    //  TODO push params to stack;
    if(!strcmp(str, "ifj16.print")){
        pushParamsToStack(symTable, stack, fn);
        Value *term = popFromStack(stack);

        print(term);

        return 0;
    }
    else if(!strcmp(str, "ifj16.readInt") ){
        // FIXME pushnut do stacku?
        Value *val = createValue(T_INTEGER);
        val->data.integer = readInt();

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.readDouble") ){
        Value *val = createValue(T_DOUBLE);
        val->data.dbl = readDouble();

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.readString") ){
        Value *val = createValue(T_STRING);
        val->data.str = readString();

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.length") ){
        pushParamsToStack(symTable, stack, fn);

        Value *val = popFromStack(stack);
        char *s = val->data.str;

        val->type = T_INTEGER;
        val->data.integer = length(s);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.substr") ){
        pushParamsToStack(symTable, stack, fn);

        Value *val = popFromStack(stack);
        char *s = val->data.str;

        val = popFromStack(stack);
        int i = val->data.integer;

        val = popFromStack(stack);
        int n = val->data.integer;

        val->type = T_STRING;
        val->data.str = substr(s, i, n);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.compare") ){
        pushParamsToStack(symTable, stack, fn);

        Value *val = popFromStack(stack);
        char *s1 = val->data.str;

        val = popFromStack(stack);
        char *s2 = val->data.str;

        val->type = T_INTEGER;
        val->data.integer = compare(s1, s2);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.sort") ){
        pushParamsToStack(symTable, stack, fn);

        Value *val = popFromStack(stack);
        char *s = val->data.str;

        val->type = T_STRING;
        val->data.str = sort(s);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.find") ){
        pushParamsToStack(symTable, stack, fn);

        Value *val = popFromStack(stack);
        char *s1 = val->data.str;

        val = popFromStack(stack);
        char *s2 = val->data.str;

        val->type = T_INTEGER;
        val->data.integer = find(s1, s2);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else
        return -1;
}

int pushParamsToStack(SymbolTable *symTable, Stack *stack, Function *fn){

    for(Declaration *dec = fn->argHead; dec != NULL; dec = dec->next){

        //find in local table
        Node *tmp = table_lookup(symTable, dec->name);
        //find in global
        if(tmp == NULL){
            tmp = table_lookup(symGlob, dec->name);
        }

        // if not found error
        if(tmp == NULL){
            PERROR("variable not found");
        }

        // push to stack
        pushToStack(stack, tmp->data.value);

    }

    return 0;
}



Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right) {

    Value *result = createValue(evalReturnType(op, left, right));
    result->undefined = true;

    // switch to right operation
    switch (op) {
        case EB_EQUAL:
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer == right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl == right->data.dbl);
                    break;
                case(T_BOOLEAN):
                    result->data.boolean = (left->data.boolean == right->data.boolean);
                    break;
                default:
                    break;
            }

            break;

        case EB_NOT_EQUAL:
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer != right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl != right->data.dbl);
                    break;
                case(T_BOOLEAN):
                    result->data.boolean = (left->data.boolean != right->data.boolean);
                    break;
                default:
                    break;
            }

            break;

        case EB_LESS:
            // return left->data < right->data;
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer < right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl < right->data.dbl);
                    break;
                case(T_BOOLEAN):
                    result->data.boolean = (left->data.boolean < right->data.boolean);
                    break;
                default:
                    break;
            }

            break;

        case EB_LESS_EQUAL:
            // return left->data <= right->data;
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer <= right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl <= right->data.dbl);
                    break;
                case(T_BOOLEAN):
                    result->data.boolean = (left->data.boolean <= right->data.boolean);
                    break;
                default:
                    break;
            }

            break;

        case EB_GREATER:
            // return left->data > right->data;
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer > right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl > right->data.dbl);
                    break;
                case(T_BOOLEAN):
                    result->data.boolean = (left->data.boolean > right->data.boolean);
                    break;
                default:
                    break;
            }

            break;

        case EB_GREATER_EQUAL:
            // return left->data >= right->data;
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer >= right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl >= right->data.dbl);
                    break;
                case(T_BOOLEAN):
                    result->data.boolean = (left->data.boolean >= right->data.boolean);
                    break;
                default:
                    break;
            }

            break;

        case EB_MULTIPLY:
            // return left->data * right->data;
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer * right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl * right->data.dbl);
                    break;
                default:
                    break;
            }
            break;

        case EB_DIVIDE:
            // return left->data / right->data
            switch(result->type){
                case(T_INTEGER):
                    if (result->data.integer == 0){
                        ERROR(ERR_RUNTIME_DIV_BY_ZERO);
                    }
                    result->data.integer = (left->data.integer / right->data.integer);
                    break;
                case(T_DOUBLE):
                    if (result->data.dbl == 0){
                        ERROR(ERR_RUNTIME_DIV_BY_ZERO);
                    }
                    result->data.dbl = (left->data.dbl / right->data.dbl);
                    break;
                default:
                    break;
            }
            break;

        case EB_ADD:
            // return left->data + right->data or concatenate strings
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer + right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl + right->data.dbl);
                    break;
                case(T_STRING):
                    result->data.str = str_cat(left->data.str, right->data.str);
                    break;
                default:
                    break;
            }
            break;

        case EB_SUBTRACT:
            // return left->data - right->data;
            switch(result->type){
                case(T_INTEGER):
                    result->data.integer = (left->data.integer - right->data.integer);
                    break;
                case(T_DOUBLE):
                    result->data.dbl = (left->data.dbl - right->data.dbl);
                    break;
                default:
                    break;
            }
            break;

    }

    result->undefined = false;

    // printf("%s\n", "~~~~~~~~~~~~~~\n\tNot fully implemented!\n~~~~~~~~~~~~~~");

    return result; //Just to pacify the compiler...
}


Value *evalStaticExpression(Expression *e) {
    switch (e->type) {
        case E_FUNCALL:
        case E_REFERENCE:
            MERROR(ERR_SYNTAX, "Cannot use dynamic expressions during static variable initialization!");
        case E_VALUE:
            return e->data.value;
        case E_BINARY:
            return evalBinaryExpression(e->data.binary.op,
                                        evalStaticExpression(e->data.binary.left),
                                        evalStaticExpression(e->data.binary.right));
    }
    return NULL; //Just to pacify the compiler...
}

Value *evalExpression(Expression *e) {
    switch (e->type) {
        case E_FUNCALL:
        dPrintf("%s","Evaluation of Funcall not implemented");
            break;
        case E_REFERENCE:
            dPrintf("%s","Evaluation of reference not implemented");
            break;
        case E_VALUE:
            return e->data.value;
        case E_BINARY:
            return evalBinaryExpression(e->data.binary.op,
                                        evalStaticExpression(e->data.binary.left),
                                        evalStaticExpression(e->data.binary.right));
    }
    return NULL; //Just to pacify the compiler...
}


ValueType evalReturnType( BinaryOperation op, Value *left, Value *right) {

    assert(left != NULL);
    assert(right != NULL);

    ValueType returnType = T_VOID;

    switch (op) {
        case EB_EQUAL:
        case EB_NOT_EQUAL:
        case EB_LESS:
        case EB_LESS_EQUAL:
        case EB_GREATER:
        case EB_GREATER_EQUAL:

            // if both arguments are equal type but are not string
            if(left->type == right->type && left->type != T_STRING){
                returnType = left->type;
            }
            // if arguments are not equal and not string
            else if(left->type != right->type && left->type != T_STRING && right->type != T_STRING){
                returnType = T_DOUBLE;
            }
            // string and boolean are not accepted here
            break;

        case EB_ADD:
            if(left->type == T_STRING || right->type == T_STRING){
                returnType = T_STRING;
                break;
            }
        case EB_MULTIPLY:
        case EB_SUBTRACT:

            if(left->type == right->type){
                returnType = left->type;
            }
            // if is at least one argument double result is also double
            else if(left->type == T_DOUBLE || right->type == T_DOUBLE){
                returnType = T_DOUBLE;
            }
            else{
                returnType = T_BOOLEAN;
            }
            break;

        case EB_DIVIDE:
           if((left->type == right->type) == T_INTEGER){
                returnType = T_INTEGER;
            }
            else{
                returnType = T_DOUBLE;
            }
            break;
    }

    return returnType;
}

char *str_cat(char *str1, char* str2){

    assert(str1 == NULL);
    assert(str2 == NULL);

    char *result = malloc_c( sizeof(char) * ( strlen(str1) + strlen(str2) ) );
    CHECK_ALLOC(result);

    unsigned x = 0;

    for ( x = 0; x < strlen(str1); x++)
        result[x] = str1[x];
    for (; x < strlen(str2) + strlen(str1); x++)
        result[x] = str2[x - strlen(str1)];

    return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~Function handling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//  Stack Init Del Push Pop

Stack *createLocalStack(Stack *stack){

    Stack *tmp = malloc_c(sizeof(Stack) + 5 * sizeof(Value*));
    CHECK_ALLOC(tmp);

    tmp->prev = stack;

    //  alocate space for 5 params.
    tmp->data[0] = NULL;

    tmp->size = -1;
    tmp->cap = 5;

    return tmp;
}

Stack *deleteLocaleStack(Stack *stack){
    assert(stack == NULL);

    Stack *tmp = stack->prev;
    // *stack = *stack->prev;

    free(stack);

    return tmp;
}

int pushToStack(Stack *stack, Value *val){
    assert(stack == NULL);
    assert(val == NULL);

    stack->size++;

    //reallocate whole stack if is no space in stack;
    if(stack->size == stack->cap){
        stack = realloc( stack, sizeof(stack) + stack->cap * 2);
        CHECK_ALLOC(stack->data);
    }

    stack->data[stack->size] = val;

    return 0;
}

Value *popFromStack(Stack *stack){
    assert(stack == NULL);

    if(stack->size == -1){
        return NULL;
    }
    else{
        return stack->data[stack->size];
    }

    return 0;
}


//  Retype

Value *reTypeFromDeclToVal(Declaration *dec){

    // Integer
    //
    errno = 0;
    char *rubbish = NULL;
    long tmp_i = strtol(dec->name, &rubbish, 10);

    if(rubbish == NULL && errno != ERANGE){
        Value *val = createValue(T_INTEGER);
        val->data.integer = tmp_i;
        return val;
    }

    // Double
    //
    rubbish = NULL;
    errno = 0;

    double tmp_d = strtof(dec->name, &rubbish);

    if(rubbish == NULL && errno != ERANGE){
        Value *val = createValue(T_DOUBLE);
        val->data.dbl = tmp_d;
        return val;
    }

    // Boolean
    //
    char *tr = "TRUE";
    char *fl = "FALSE";

    if(!strcmp(dec->name,tr)){
        Value *val = createValue(T_BOOLEAN);
        val->data.boolean = 1;
        return val;
    }
    else if(!strcmp(dec->name,fl)){
        Value *val = createValue(T_BOOLEAN);
        val->type = T_BOOLEAN;
        return val;
    }

    return NULL;

}

int reType(Value *to, Value *from, ValueType type){

    if(to == NULL || from == NULL)
        return -1;

    if(type != T_VOID)
        return -1;


    return 0;
}
