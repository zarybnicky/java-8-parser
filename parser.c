/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "parser.h"

Token *peekToken(Lexer *l) {
    if (l->start == NULL && l->current == NULL) {
        l->start = l->current = getNextToken(l->file);
    }
    if (l->current != NULL && l->current->next == NULL) {
        l->current->next = getNextToken(l->file);
    }
    return l->current;
}

Token *getToken(Lexer *l) {
    Token *t = peekToken(l);
    nextToken(l);
    return t;
}

void consume(Lexer *l) {
    while (l->start != l->current) {
        Token *t = l->start;
        l->start = t->next;
        freeToken(t);
    }
}

void class(Lexer *l) {
    expectReserved(l, RES_CLASS);
    printf("Found a class %s\n", simpleId(l));
    expectSymbol(l, SYM_BRACE_OPEN);
    while (classComponent(l))
        ;
    expectSymbol(l, SYM_BRACE_CLOSE);
    consume(l);
}

bool classComponent(Lexer *l) {
    if (isSymbol(l, SYM_BRACE_CLOSE)) {
        return false;
    }
    expectReserved(l, RES_STATIC);
    ValueType ret = returnType(l);
    char *id = simpleId(l);

    expectType(l, SYMBOL);
    Token *t = getToken(l);
    switch (t->symbol) {
    case SYM_SEMI:
        if (ret == T_VOID) {
            fprintf(stderr, "Unexpected void type on line %d, char %d.\n",
                    t->lineNum, t->lineChar);
            exit(ERR_SYNTAX);
        }
        printf("Found an uninitialized variable %s of type %d\n", id, ret);
        return true;
    case SYM_ASSIGN:
        if (ret == T_VOID) {
            fprintf(stderr, "Unexpected void type on line %d, char %d.\n",
                    t->lineNum, t->lineChar);
            exit(ERR_SYNTAX);
        }
        //expr
        printf("Found an initialized variable %s of type %d\n", id, ret);
        return true;
    case SYM_PAREN_OPEN:
        //argList
        expectSymbol(l, SYM_PAREN_CLOSE);
        expectSymbol(l, SYM_BRACE_OPEN);
        while (functionComponent(l))
            ;
        expectSymbol(l, SYM_BRACE_CLOSE);
        printf("Found a function %s of return type %d\n", id, ret);
        return true;
    default:
        fprintf(stderr, "Unexpected symbol on line %d, char %d.\n",
                t->lineNum, t->lineChar);
        exit(ERR_SYNTAX);
    }
}

bool functionComponent(Lexer *l) {
    if (isSymbol(l, SYM_BRACE_CLOSE)) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        printToken(getToken(l));
    }
    return true;
}

char *simpleId(Lexer *l) {
    expectType(l, ID_SIMPLE);
    return getToken(l)->simpleId;
}

ValueType returnType(Lexer *l) {
    expectType(l, RESERVED);
    Token *t = getToken(l);
    switch (t->reserved) {
    case RES_BOOLEAN: return T_BOOLEAN;
    case RES_DOUBLE: return T_DOUBLE;
    case RES_INT: return T_INTEGER;
    case RES_STRING: return T_STRING;
    case RES_VOID: return T_VOID;
    default:
        fprintf(stderr, "Expected a type on line %d, char %d.\n",
                t->lineNum, t->lineChar);
        exit(ERR_SYNTAX);
    }
}
