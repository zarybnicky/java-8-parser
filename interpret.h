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

/*---------------------------------LIBRARIES----------------------------------*/

// System libraries
#include <stdio.h>
#include <stdlib.h>

// Local libraries
#include "error.h"
#include "int_memory_management.h"
#include "ir.h"
#include "ial.h"

/*---------------------------------STRUCTURES---------------------------------*/

// typedef int *something;

/*---------------------------------VARIABLES----------------------------------*/


/*----------------------------------MACROS------------------------------------*/

// HALT

#define HALT 0

typedef struct {
    SymbolTable symTable;
} Interpret;


/*--------------------------------DEFINITIONS---------------------------------*/

int int_initialize(void);
int int_looper(int *instruction);

Interpret *createInterpret();
void freeInterpret(Interpret *);

void evalMain(Interpret *);

//Static variables can be initialiezd using an expression as well, though I
//suppose these expressions shouldn't contain any references to variables?
Value *evalStaticExpression(Expression *);
Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right);

#endif
