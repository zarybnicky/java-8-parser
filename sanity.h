/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_SANITY_H
#define IFJ_SANITY_H

#include <stdlib.h>
#include "error.h"
#include "ir.h"
#include "ial.h"
#include "interpret.h"

void runSemanticAnalysis(Interpret *);

void checkReturnCommand(Function *, Command *);
void checkReturnType(Node *);

//support function for fnExpression definition??!
void checkFnExpresion(Function *,Command *);

//Top level only??
void checkTopLevel(Node *);
void checkAllStatic (Node *);
void checkMainRun(SymbolTable *);

void checkOperatorAssignmentType(Node *);

#endif /* IFJ_SANITY_H */
