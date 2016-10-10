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
#include <string.h>

typedef enum {
    RESERVED,
    ID_SIMPLE,
    ID_COMPOUND,
    LIT_INTEGER,
    LIT_STRING,
    LIT_BOOLEAN,
    LIT_DOUBLE,
} TokenType;

typedef enum {
    RES_BOOLEAN,
    RES_BREAK,
    RES_CLASS,
    RES_CONTINUE,
    RES_DO,
    RES_DOUBLE,
    RES_ELSE,
    RES_FALSE,
    RES_FOR,
    RES_IF,
    RES_INT,
    RES_RETURN,
    RES_STRING,
    RES_STATIC,
    RES_TRUE,
    RES_VOID,
    RES_WHILE,
} ReservedWord;

typedef struct {
    int lineNum;
    int lineChar; //For error reporting
    TokenType type;
    union {
        struct { ReservedWord reserved; };           //RESREVED
        struct { char *localId; };                   //ID_SIMPLE
        struct { char *namespace; char *staticId; }; //ID_COMPOUND
        struct { int intValue; };                    //LIT_INTEGER
        struct { double doubleValue; };              //LIT_DOUBLE
        struct { char boolValue; };                  //LIT_BOOLEAN
        struct { char *stringValue; };               //LIT_STRING
    };
} Token;

Token getNextToken(FILE *stream);

#endif /* IFJ_SCANNER_H */
