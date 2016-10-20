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
#include <semaphore.h>

// Local libraries
#include "error.h"
<<<<<<< HEAD
#include "int_memory_management.h"
=======
#include "ir.h"
#include "ial.h"
>>>>>>> refs/remotes/origin/master

/*---------------------------------STRUCTURES---------------------------------*/

// typedef int *something;

/*---------------------------------VARIABLES----------------------------------*/

sem_t sem_interpret;

/*----------------------------------MACROS------------------------------------*/

// HALT

#define HALT 0


//  Math operations

#define INS_ADD 1
#define INS_SUB 2
#define INS_MUL 3
#define INS_DIV 4
#define INS_MOD 5


//  Logical operations

<<<<<<< HEAD
#define INS_NOT 10
#define INS_AND 11
#define INS_OR  12
#define INS_XOR 13
=======
typedef struct {
    SymbolTable symTable;
} Interpret;
>>>>>>> refs/remotes/origin/master


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
