/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_TYPE_H
#define IFJ_TYPE_H

#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include "error.h"
#include "ir.h"
#include "ial.h"

ValueType getBinExpType(BinaryOperation, ValueType, ValueType);
bool isAssignCompatible(ValueType, ValueType);
Value *coerceTo(ValueType, Value *);

#endif /* IFJ_TYPE_H */
