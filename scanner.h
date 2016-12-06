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
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include "error.h"
#include "ir.h"

#define ALLOC_BLOCK 2
//int alloc_length = ALLOC_BLOCK;

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
    SYM_UNARY_PLUS,   //++
    SYM_UNARY_MINUS,  //--
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
    SYM_NOT,          //!
    SYM_AND,          //&&
    SYM_OR,           //||
} SymbolType;

typedef enum{
    NEUTRAL_STATE,
    Start_state,
    RESERVED_WORD,
    END_OF_FILE,
    AUT_SYMBOL,
    AUT_IDEN,
    AUT_IDEN2,
    AUT_IDEN3,
    AUT_NUM,
    AUT_FLOAT1 ,
    AUT_FLOAT2,
    AUT_EX1,
    AUT_EX2,
    AUT_EX3,
    AUT_STRING,
    AUT_ESC,
    AUT_ESC_ZERO,
    AUT_ESCN,
    AUT_ESC_ZERO2,
    AUT_ESCN2,
    AUT_DIV,
    AUT_DIV2,
    AUT_CMTL,
    AUT_CMTB,
    AUT_CMTB_END,
    AUT_EQUALS,
    AUT_LESS,
    AUT_GREAT,
    AUT_NOT_EQUALS,
    AUT_UNARY_PL,
    AUT_UNARY_MN,
    AUT_AND,
    AUT_OR,
    AUT_BIN,
    AUT_BIN2,
    AUT_HEX,
    AUT_OCT,
    NUMBER,
    FLOAT,
    ERROR_IDEN,
    ERROR_R,
    ERROR_NUMBER,
    ERROR_ASCII,
    ERROR_ESC,
    ERROR_ESC_ZERO,
    ERROR_ESCN,
    ERROR_ESC_ZERO2,
    ERROR_ESCN2,
    ERROR_CMTB,
    ERROR_NOT_EQUALS,
    ERROR_AND,
    ERROR_OR,
    IDEN,
    IDEN_COMPOUND,
    STRING,
    EXT_BASE,
    EXT_HEX,
    EXT_OCT,
}AUTSTATES;

typedef struct s_Token {
    //For error reporting
    int lineNum;
    int lineChar;
    char *original; //The token as it appeared in the input stream

    //For parsing
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
void string_end(char **string, char c, int *stringLength, int *stringAlloc);
Token *createToken(TokenType, void *, char *);
Token *chainTokens(Token *, ...);
void freeToken(Token *t);
void printToken(Token *t);
AUTSTATES Get_Token(FILE *input, char **string, ReservedWord *reser, SymbolType *symbol, int *lineNumToken, int *lineColToken);

#endif /* IFJ_SCANNER_H */
