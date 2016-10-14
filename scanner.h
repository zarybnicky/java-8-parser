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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef enum {
    RESERVED,
    SYMBOL,
    ID_SIMPLE,
    ID_COMPOUND,
    LIT_INTEGER,
    LIT_STRING,
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

typedef enum {
    SYM_BRACE_OPEN,   //{
    SYM_BRACE_CLOSE,  //}
    SYM_BRACKET_OPEN, //[
    SYM_BRACKET_CLOSE,//]
    SYM_PAREN_OPEN,   //(
    SYM_PAREN_CLOSE,  //)
    SYM_PLUS,         //+
    SYM_MINUS,        //-
    SYM_STAR,         //*
    SYM_SLASH,        ///
    SYM_SEMI,         //;
    SYM_COMMA,        //,
    SYM_GREATER,      //>
    SYM_GREATER_EQUAL,//>=
    SYM_LESS,         //<
    SYM_LESS_EQUAL,   //<=
    SYM_EQUALS,       //==
    SYM_NOT_EQUALS,   //!=
    SYM_ASSIGN,       //=
} SymbolType;

typedef struct s_Token {
    int lineNum;
    int lineChar; //For error reporting
    TokenType type;
    struct s_Token *next; //For parsing only, not for lexing
    union {
        ReservedWord reserved;
        SymbolType symbol;
        char *id; //both for ID_SIMPLE & ID_COMPOUND
        int intVal;
        double doubleVal;
        bool boolVal;
        char *stringVal;
    } val;
} Token;

Token *getNextToken(FILE *stream);
void printToken(Token *t);
void freeToken(Token *t);

#endif /* IFJ_SCANNER_H */
