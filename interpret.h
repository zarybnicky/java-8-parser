/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef INTERPRET_H
#define INTERPRET_H

/*--------------------------------LIBRARIES---------------------------------*/


// System libraries
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>

// Local libraries
#include "error.h"
#include "ial.h"
#include "ir.h"
#include "int_memory_management.h"
#include "int_debug.h"
#include "type.h"

/*--------------------------------STRUCTURES--------------------------------*/

typedef struct {
    SymbolTable symTable;
} Interpret;

typedef struct Stack{
    struct Stack *prev;
    int cap;
    int size;
    Value *data[];
} Stack;

/*-------------------------------Declarations-------------------------------*/


Interpret *createInterpret(void);
int freeInterpret(Interpret *);

//  Main interpret function
int evalMain(Interpret *);

//Static variables can be initialiezd using an expression as well, though I
//suppose these expressions shouldn't contain any references to variables?

int interpretNode(Stack *, Node *);
int interpretFunc(Stack *, Node *);
int builtInFunc(Stack *, Function *);

// These functions are needed to evaluate abstract syntax tree.
Value *evalStaticExpression(Expression *);
bool evalCondition(SymbolTable *, Stack *, char *, Expression *);
Value *evalExpression(SymbolTable *, Stack *, char *, Expression *, UnaryOperation);
Value *evalCommand(SymbolTable *, Stack *, Command *, char *);
int evalBlock(SymbolTable *, Stack *, Block *, char *);
void evalFunction(Stack *, SymbolTable *, Function *, char *);
Value *evalUnaryExpression(UnaryOperation, Value *);
Value *evalBinaryExpression(BinaryOperation, Value *, Value *);

// Stack constructor
Stack *createLocalStack(Stack *);

// Work with Stack
int pushToStack(Stack *, Value *);
Value *popFromStack(Stack *);
int pushParamToStack(SymbolTable *, Stack *, char *, Expression *);
void printStack(Stack *);
Stack *createLocalStack(Stack *stack);
int pushToStack(Stack *stack, Value *val);
Value *popFromStack(Stack *stack);

#endif
