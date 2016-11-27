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

typedef enum{
    FN_PRINT,
    FN_READINT,
    FN_READDBL,
    FN_READSTR,
    FN_LENGTH,
    FN_SUBSTR,
    FN_COMPARE,
    FN_SORT,
    FN_FIND,
    FN_OTHER
} FnTypes;

/*-------------------------------Declarations-------------------------------*/


Interpret *createInterpret(void);

int freeInterpret(Interpret *);


int evalMain(Interpret *);

//Static variables can be initialiezd using an expression as well, though I
//suppose these expressions shouldn't contain any references to variables?

int interpretNode(Stack *, Node *);
int interpretFunc(Stack *, Node *);
int builtInFunc(SymbolTable *, Stack *, Function *);

Value *evalStaticExpression(Expression *);
bool evalCondition(SymbolTable *, Stack *, char *, Expression *);
Value *evalExpression(SymbolTable *, Stack *, char *, Expression *);
Value *evalCommand(SymbolTable *, Stack *, Command *, char *);
int evalBlock(SymbolTable *, Stack *, Block *, char *);
Value *evalFunction(Stack *,SymbolTable *,char *,int,Expression *,char *);

Value *evalBinaryExpression(BinaryOperation, Value *, Value *);

Value *coerceTo(ValueType, Value *);

Stack *createLocalStack(Stack *);
Stack *deleteLocaleStack(Stack *);

int pushToStack(Stack *, Value *);
Value *popFromStack(Stack *);
int pushParamToStack(SymbolTable *, Stack *, char *, Expression *);

void testStack();
void printStack(Stack *);
Stack *createLocalStack(Stack *stack);
Stack *deleteLocaleStack(Stack *stack);
int pushToStack(Stack *stack, Value *val);
Value *popFromStack(Stack *stack);

#endif
