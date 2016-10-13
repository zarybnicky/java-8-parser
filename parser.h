/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_PARSER_H
#define IFJ_PARSER_H

#include "types.h"
#include "scanner.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    FILE *file;
    Token *start;
    Token *current;
    unsigned stackSize;
    unsigned stackPtr;
    Token **stack;
    char *lastClassName;
} Lexer;

Lexer *createLexer(FILE *);
bool try(Lexer *l);
bool rollback(Lexer *l);
bool commit(Lexer *l);

#define bracket(l, fn, ...)                     \
    try(l);                                     \
    if (fn(l, ## __VA_ARGS__)) {                \
        commit(l);                              \
        return true;                            \
    }                                           \
    rollback(l);
#define bracket_(l, fn)                         \
    try(l);                                     \
    if (fn(l)) {                                \
        commit(l);                              \
        return true;                            \
    }                                           \
    rollback(l);


#define STRINGIFY(arg)  STRINGIFY1(arg)
#define STRINGIFY1(arg) STRINGIFY2(arg)
#define STRINGIFY2(arg) #arg

#define expectMore(l)                                               \
        if (peekToken(l) == NULL) {                                 \
            fprintf(stderr, "Unexpected end of input.\n");          \
            exit(ERR_SYNTAX);                                       \
        }
#define expectType(l, x) do {                                       \
        expectMore(l);                                              \
        Token *t = peekToken(l);                                    \
        if (t->type != (x)) {                                       \
            fprintf(stderr, "Expected %s on line %d, char %d.\n",   \
                    STRINGIFY(x), t->lineNum, t->lineChar);         \
            exit(ERR_SYNTAX);                                       \
        }                                                           \
    } while (0);
#define expectSymbol(l, x) do {                                     \
        expectMore(l);                                              \
        Token *t = peekToken(l);                                    \
        if (t->type != SYMBOL || t->val.symbol != (x)) {            \
            fprintf(stderr, "Expected %s on line %d, char %d.\n",   \
                    STRINGIFY(x), t->lineNum, t->lineChar);         \
            exit(ERR_SYNTAX);                                       \
        }                                                           \
        nextToken(l);                                               \
    } while (0);
#define expectReserved(l, x) do {                                   \
        expectMore(l);                                              \
        Token *t = peekToken(l);                                    \
        if (t->type != RESERVED || t->val.reserved != (x)) {        \
            fprintf(stderr, "Expected %s on line %d, char %d.\n",   \
                    STRINGIFY(x), t->lineNum, t->lineChar);         \
            exit(ERR_SYNTAX);                                       \
        }                                                           \
        nextToken(l);                                               \
    } while (0);
#define tryType(l, x, ret) do {                                         \
        Token *t = peekToken(l);                                        \
        if (t == NULL || t->type != (x)) {                              \
            return ret;                                                 \
        }                                                               \
    } while (0);
#define trySymbol(l, x, ret) do {                                       \
        Token *t = peekToken(l);                                        \
        if (t == NULL || t->type != SYMBOL || t->val.symbol != (x)) {   \
            return ret;                                                 \
        }                                                               \
        nextToken(l);                                                   \
    } while (0);
#define tryReserved(l, x, ret) do {                                     \
        Token *t = peekToken(l);                                        \
        if (t == NULL || t->type != RESERVED || t->val.reserved != (x)) { \
            return ret;                                                 \
        }                                                               \
        nextToken(l);                                                   \
    } while (0);
#define tryValueType(l, ret) do {\
        Token *t = peekToken(l);                                        \
        if (t == NULL || t->type != RESERVED ||                         \
            !(t->val.reserved == RES_BOOLEAN ||                         \
              t->val.reserved == RES_DOUBLE ||                          \
              t->val.reserved == RES_INT ||                             \
              t->val.reserved == RES_STRING)) {                         \
            return ret;                                                 \
        }                                                               \
        nextToken(l);                                                   \
    } while (0);

#define hasMore(l) \
    (peekToken(l) != NULL)
#define isType(l, x) \
    (peekToken(l) != NULL && peekToken(l)->type == (x))
#define isSymbol(l, x) \
    (peekToken(l) != NULL && peekToken(l)->type == SYMBOL && peekToken(l)->val.symbol == (x))
#define isReserved(l, x) \
    (peekToken(l) != NULL && peekToken(l)->type == RESERVED && peekToken(l)->val.reserved == (x))
#define nextToken(l)  if (peekToken(l) != NULL) (l)->current = (l)->current->next;

Token *peekToken(Lexer *l);

void parseClass(Lexer *l);
bool parseClassBody(Lexer *l);
bool parseStaticVar(Lexer *l);
bool parseStaticVarInit(Lexer *l);
bool parseFunction(Lexer *l);
bool parseFunctionBody(Lexer *l, Block *f);

bool parseVarDeclaration(Lexer *l, Block *b);
bool parseVarDefinition(Lexer *l, Block *b);
bool parseIf(Lexer *l, Block *b);
bool parseWhile(Lexer *l, Block *b);
bool parseCommand(Lexer *l, Block *b);

bool parseAssign(Lexer *l, Block *b);
bool parseFuncall(Lexer *l, Block *b);
bool parseReturn(Lexer *l, Block *b);
bool parseBlock(Lexer *l, Block *b);

bool parseExpression(Lexer *l, Expression **e);
bool parseExpressionCmp(Lexer *l, Expression **e);
bool parseExpressionMul(Lexer *l, Expression **e);
bool parseExpressionAdd(Lexer *l, Expression **e);
bool parseExpressionTerm(Lexer *l, Expression **e);
bool parseExpressionParen(Lexer *l, Expression **e);
bool parseExpressionFuncall(Lexer *l, Expression **e);
bool parseExpressionLiteral(Lexer *l, Expression **e);
bool parseExpressionVar(Lexer *l, Expression **e);

bool parseDeclaration(Lexer *l, Declaration **d);
Declaration *parseArgListDecl(Lexer *l, int *argCount);
Expression *parseArgListCall(Lexer *l, int *argCount);

char *parseSimpleId(Lexer *l);
char *parseAndQualifyId(Lexer *l);
ValueType parseType(Lexer *l);
ValueType parseReturnType(Lexer *l);

void appendToBlock(Block *, Command *);
void freeBlock(Block);

Value *createValue(ValueType);
void freeValue(Value *);
void printValue(Value *);

Expression *createExpression(ExpressionType);
void freeExpression(Expression *);
void printExpression(Expression *);

Declaration *createDeclaration(ValueType, char *);
void freeDeclaration(Declaration *);
void printDeclaration(Declaration *);

Command *createCommand(CommandType);
void freeCommand(Command *);
void printCommand(Command *);

Function *createFunction(char *, int, Declaration *);
void freeFunction(Function *);
void printFunction(Function *);

#endif /* IFJ_PARSER_H */
