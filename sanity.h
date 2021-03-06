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
#include "int_memory_management.h"
#include "type.h"

void runSemanticAnalysis(SymbolTable *);

void checkReturnPresenceC(Function *, Command *);
void checkReturnPresence(Function *);
void checkMainRunPresence();

void checkOperatorAssignmentTypeC(Function *, Command *);
void checkOperatorAssignmentTypeF(Command *);
void checkOperatorAssignmentType(Function *);
void checkTopLevelInner(Function *, Command *, bool cycle);
void checkTopLevel(Function *);

ValueType getExpressionType(Expression *, bool hasString);
bool hasString(Expression *);

#endif /* IFJ_SANITY_H */
