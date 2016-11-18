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

#include "interpret.h"


Interpret *createInterpret(void) {
    Interpret *i = malloc(sizeof(Interpret));
    CHECK_ALLOC(i);
    i->symTable.root = NULL;
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
    Node *mainFn = table_lookup(&i->symTable, "Main.run");
    if (mainFn == NULL || mainFn->type != N_FUNCTION) {
        MERROR(ERR_SEM_UNDEFINED, "Required method Main.run is not defined");
    }

    Stack *GlobalStack = NULL;
    GlobalStack = createLocalStack(NULL);

    //  Push arguments to stack

    for(Declaration *dec = i->symTable.root->data.function->argHead;
        dec->next != NULL;
        dec = dec->next)
    {
        Value *tmp = reTypeFromDeclToVal(dec);
        printf("%s: %s: tmp: %d",__FILE__, __func__, tmp->type);

    }

    free(GlobalStack);

    // interpret(i->symTable.root->data.function);


    // sem zavolaj ten while s tym velkym switchom;

    return 0;
}


Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right) {

    // memory allocation for result
    Value *result = malloc(sizeof(Value));
    CHECK_ALLOC(result);

    // calculate result data type
    result->type = evalReturnType(op, left, right);

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
                case(T_STRING):
                case(T_VOID):
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
                case(T_STRING):
                case(T_VOID):
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
                case(T_STRING):
                case(T_VOID):
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
                case(T_STRING):
                case(T_VOID):
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
                case(T_STRING):
                case(T_VOID):
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
                case(T_STRING):
                case(T_VOID):
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
                case(T_STRING):
                case(T_VOID):
                case(T_BOOLEAN):
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

                case(T_STRING):
                case(T_VOID):
                case(T_BOOLEAN):
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
                case(T_VOID):
                case(T_BOOLEAN):
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
                case(T_STRING):
                case(T_VOID):
                case(T_BOOLEAN):
                    break;
            }
            break;

    }

    result->undefined = false;

    // printf("%s\n", "~~~~~~~~~~~~~~\n\tNot fully implemented!\n~~~~~~~~~~~~~~");

    return result; //Just to pacify the compiler...
}


Value *evalStaticExpression(Expression *e) {
    Value *left, *right;

    switch (e->type) {
        case E_FUNCALL:
        case E_REFERENCE:
            MERROR(ERR_SYNTAX, "Cannot use dynamic expressions during static variable initialization!");
        case E_VALUE:
            return e->data.value;
        case E_BINARY:
            left = evalStaticExpression(e->data.binary.left);
            right = evalStaticExpression(e->data.binary.right);
            return evalBinaryExpression(e->data.binary.op, left, right);
    }
    return NULL; //Just to pacify the compiler...
}


ValueType evalReturnType( BinaryOperation op, Value *left, Value *right) {

    // (void*) left;
    // (void*) right; // turn off compiler warnings for arguments

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

    char *result = malloc( sizeof(char) * ( strlen(str1) + strlen(str2) ) );
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

    Stack *tmp = malloc(sizeof(Stack) + 5 * sizeof(Value*));
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

int pustToStack(Stack *stack, Value *val){
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

    Value *val = (Value*)malloc_c(sizeof(Value));
    val->undefined = 0;

    // Integer
    //
    errno = 0;
    char *rubbish = NULL;
    long tmp_i = strtol(dec->name, &rubbish, 10);

    if(rubbish == NULL && errno != ERANGE){
        val->data.integer = tmp_i;
        val->type = T_INTEGER;
        return val;
    }

    // Double
    //
    rubbish = NULL;
    errno = 0;

    double tmp_d = strtof(dec->name, &rubbish);

    if(rubbish == NULL && errno != ERANGE){
        val->data.dbl = tmp_d;
        val->type = T_DOUBLE;
        return val;
    }

    // Boolean
    //
    char *tr = "TRUE\0";
    char *fl = "FALSE\0";

    if(!strcmp(dec->name,tr)){
        val->data.boolean = 1;
        val->type = T_BOOLEAN;
        return val;
    }
    else if(!strcmp(dec->name,fl)){
        val->data.boolean = 0;
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
