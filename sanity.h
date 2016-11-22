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

void checkReturnPresenceC(Function *, Command *);
void checkReturnPresence(Node *);
void checkReturnCommand(Function *, Command *);
void checkReturnType(Node *);

//support function for fnExpression definition??!
char *changeFunctionName(Expression *);
void checkBinaryReference_(char *,char *);
void checkBinaryCond_(Expression *);
Expression *expLoopSwitch(Command *);
void checkCondition_(Expression *, Function *);
void checkFnExpression(Function *,Command *);
//Sanity main local/global variables functions
void checkAllStatic (Node *);
void checkMainRun(SymbolTable *);

ValueType coerceBinary(BinaryOperation, ValueType, ValueType);
bool checkAssignCompatible(ValueType, ValueType);
ValueType getExpressionType(Expression *);
void checkOperatorAssignmentTypeC(Function *, Command *);
void checkOperatorAssignmentTypeF(Command *);
void checkOperatorAssignmentType(Node *);
void checkTopLevelInner(Function *, Command *);
void checkTopLevel(Node *);

#endif /* IFJ_SANITY_H */
