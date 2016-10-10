/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H

#include "types.h"
#include <stdio.h>

typedef enum {
    Integer, Double, string, Type, SimpleId, CompoundId, Reserved, Symbol
} TokenType;

typedef struct {
    int lineNum;
    int lineChar; //For error reporting
    TokenType type;
    union {
        struct { int intValue; };
        struct { double doubleValue; };
        struct { char *stringValue; };
        struct { char *typeValue; };
        struct { char *localId; };
        struct { char *namespace; char *staticId; };
        struct { char *reservedValue; };
        struct { char *symbolValue; };
    };
} Token;

Token getNextToken(FILE *stream);

#endif /* IFJ_SCANNER_H */
