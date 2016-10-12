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

typedef struct {
    FILE *file;
    Token *start;
    Token *current;
} Lexer;

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
        if (t->type != SYMBOL || t->symbol != (x)) {                \
            fprintf(stderr, "Expected %s on line %d, char %d.\n",   \
                    STRINGIFY(x), t->lineNum, t->lineChar);         \
            exit(ERR_SYNTAX);                                       \
        }                                                           \
        nextToken(l);                                               \
    } while (0);
#define expectReserved(l, x) do {                                   \
        expectMore(l);                                              \
        Token *t = peekToken(l);                                    \
        if (t->type != RESERVED || t->reserved != (x)) {            \
            fprintf(stderr, "Expected %s on line %d, char %d.\n",   \
                    STRINGIFY(x), t->lineNum, t->lineChar);         \
            exit(ERR_SYNTAX);                                       \
        }                                                           \
        nextToken(l);                                               \
    } while (0);

#define hasMore(l) \
    (peekToken(l) != NULL)
#define isType(l, x) \
    (peekToken(l) != NULL && peekToken(l)->type == (x))
#define isSymbol(l, x) \
    (peekToken(l) != NULL && peekToken(l)->type == SYMBOL && peekToken(l)->symbol == (x))
#define isReserved(l, x) \
    (peekToken(l) != NULL && peekToken(l)->type == RESERVED && peekToken(l)->reserved == (x))
#define nextToken(l)  if (peekToken(l) != NULL) (l)->current = (l)->current->next;

Token *peekToken(Lexer *l);
void consume(Lexer *l);

void class(Lexer *l);
bool classComponent(Lexer *l);
bool functionComponent(Lexer *l);
char *simpleId(Lexer *l);
ValueType returnType(Lexer *l);

#endif /* IFJ_PARSER_H */
