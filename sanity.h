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

void runSemanticAnalysis(Interpret *i);

void checkReturnCommand(Function *, Command *);
void checkReturnType(Node *);

void checkFnExpresion(Function *,Expression *);

//Top level only??
void checkTopLevel(Node *node);
void checkMainRun(Node *node);

#endif /* IFJ_SANITY_H */