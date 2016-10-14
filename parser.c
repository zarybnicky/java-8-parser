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

Lexer *createLexer(FILE *f) {
    Lexer *l = malloc(sizeof(Lexer));
    CHECK_ALLOC(l);
    l->file = f;
    l->start = l->current = NULL;
    l->stackSize = 20;
    l->stackPtr = 0;
    l->stack = malloc(l->stackSize * sizeof(Token *));
    CHECK_ALLOC(l->stack);
    l->lastClassName = NULL;
    return l;
}

bool try(Lexer *l) {
    if (l->stackPtr == l->stackSize) {
        l->stackSize *= 2;
        l->stack = realloc(l->stack, l->stackSize * sizeof(Token *));
        CHECK_ALLOC(l->stack);
    }
    l->stack[l->stackPtr++] = l->current;
    return true;
}
bool rollback(Lexer *l) {
    if (l->stackPtr > 0) {
        l->current = l->stack[--l->stackPtr];
    }
    return true;
}
bool commit(Lexer *l) {
    if (l->stackPtr > 0) {
        --l->stackPtr;
    } else {
        while (l->start != l->current) {
            Token *t = l->start;
            l->start = t->next;
            freeToken(t);
        }
    }
    return true;
}

void parseClass(Lexer *l) {
    expectReserved(l, RES_CLASS);
    char *name = parseSimpleId(l);
    expectSymbol(l, SYM_BRACE_OPEN);

    l->lastClassName = name;
    printf("Found a class %s\n", name);
    while (parseClassBody(l))
        ;
    expectSymbol(l, SYM_BRACE_CLOSE);
    commit(l);
}

bool parseClassBody(Lexer *l) {
    bracket_(l, parseFunction);
    bracket_(l, parseStaticDeclaration);
    bracket_(l, parseStaticDefinition);
    return false;
}

bool parseStaticDeclaration(Lexer *l) {
    tryReserved(l, RES_STATIC, false);
    ValueType type = parseType(l);
    char *name = parseSimpleId(l);
    trySymbol(l, SYM_SEMI, (free(name), false));
    printf("Found an uninitialized variable %s of type %d\n", name, type);
    return true;
}

bool parseStaticDefinition(Lexer *l) {
    tryReserved(l, RES_STATIC, false);
    ValueType type = parseType(l);
    char *name = parseSimpleId(l);
    trySymbol(l, SYM_ASSIGN, (free(name), false));
    //expr
    printf("Found an initialized variable %s of type %d\n", name, type);
    return true;
}

bool parseFunction(Lexer *l) {
    tryReserved(l, RES_STATIC, false);
    ValueType type = parseReturnType(l);
    char *name = parseSimpleId(l);
    trySymbol(l, SYM_PAREN_OPEN, (free(name), false));

    int argCount = 0;
    Declaration *argList = parseArgListDecl(l, &argCount);
    expectSymbol(l, SYM_PAREN_CLOSE);
    expectSymbol(l, SYM_BRACE_OPEN);

    Function *f = createFunction(name, argCount, argList);
    while (parseFunctionBody(l, &f->body))
        ;
    expectSymbol(l, SYM_BRACE_CLOSE);

    printf("Found a function %s of return type %d\n", name, type);
    freeFunction(f); //FIXME: register function instead

    return true;
}

bool parseFunctionBody(Lexer *l, Block *b) {
    bracket(l, parseLocalDeclaration, b);
    bracket(l, parseLocalDefinition, b);
    bracket(l, parseIf, b);
    bracket(l, parseWhile, b);
    bracket(l, parseCommand, b);
    return false;
}

bool parseLocalDeclaration(Lexer *l, Block *b) {
    Command *c = createCommand(C_DECLARE);
    Declaration *d = &c->data.declare;
    if (!parseDeclaration(l, &d)) {
        return false;
    }
    trySymbol(l, SYM_SEMI, (freeCommand(c), false));
    appendToBlock(b, c);
    return true;
}

bool parseLocalDefinition(Lexer *l, Block *b) {
    Command *c = createCommand(C_DECLARE);
    Declaration *d = &c->data.declare;
    parseDeclaration(l, &d);
    if (!parseDeclaration(l, &d)) {
        return false;
    }
    expectSymbol(l, SYM_ASSIGN);
    Expression *e = NULL;
    if (!parseExpression(l, &e)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    c->data.define.expr = e;
    appendToBlock(b, c);
    return true;
}

bool parseIf(Lexer *l, Block *b) {
    tryReserved(l, RES_IF, false);
    expectSymbol(l, SYM_PAREN_OPEN);
    Expression *e = NULL;
    if (!parseExpression(l, &e)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    expectSymbol(l, SYM_PAREN_CLOSE);

    Command *c = createCommand(C_IF);
    c->data.ifC.cond = e;
    if (!parseCommand(l, &c->data.ifC.thenBlock)) {
        FERROR(ERR_SYNTAX,
               "Expected a command on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    appendToBlock(b, c);
    tryReserved(l, RES_ELSE, true);
    if (!parseCommand(l, &c->data.ifC.elseBlock)) {
        FERROR(ERR_SYNTAX,
               "Expected a command on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    return true;
}

bool parseWhile(Lexer *l, Block *b) {
    tryReserved(l, RES_WHILE, false);
    expectSymbol(l, SYM_PAREN_OPEN);
    Expression *e = NULL;
    if (!parseExpression(l, &e)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    expectSymbol(l, SYM_PAREN_CLOSE);

    Command *c = createCommand(C_WHILE);
    c->data.whileC.cond = e;
    if (!parseCommand(l, &c->data.whileC.bodyBlock)) {
        FERROR(ERR_SYNTAX,
               "Expected a command on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    appendToBlock(b, c);
    return true;
}

bool parseCommand(Lexer *l, Block *b) {
    bracket(l, parseAssign, b);
    bracket(l, parseFuncall, b);
    bracket(l, parseReturn, b);
    bracket(l, parseBlock, b);
    return false;
}

bool parseAssign(Lexer *l, Block *b) {
    Token *t = peekToken(l);
    if (t->type != ID_SIMPLE && t->type != ID_COMPOUND)
        return false;
    char *name = parseAndQualifyId(l);
    trySymbol(l, SYM_ASSIGN, (free(name), false));
    Command *c = createCommand(C_ASSIGN);
    c->data.assign.name = name;
    Expression *e = NULL;
    if (!parseExpression(l, &e)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    c->data.assign.expr = e;
    appendToBlock(b, c);
    return true;
}

bool parseFuncall(Lexer *l, Block *b) {
    Expression *e = NULL;
    if (!parseExpressionFuncall(l, &e)) {
        return false;
    }
    expectSymbol(l, SYM_SEMI);
    Command *c = createCommand(C_EXPRESSION);
    c->data.expr = e;
    appendToBlock(b, c);
    return true;
}

bool parseReturn(Lexer *l, Block *b) {
    tryReserved(l, RES_RETURN, false);
    Command *c = createCommand(C_RETURN);
    if (isSymbol(l, SYM_SEMI)) {
        c->data.expr = NULL;
        appendToBlock(b, c);
        return true;
    }
    Expression *e = NULL;
    if (!parseExpression(l, &e)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    c->data.expr = e;
    appendToBlock(b, c);
    return true;
}

bool parseBlock(Lexer *l, Block *b) {
    trySymbol(l, SYM_BRACE_OPEN, false);
    Command *c = createCommand(C_BLOCK);
    while (parseFunctionBody(l, &c->data.block))
        ;
    expectSymbol(l, SYM_BRACE_CLOSE);
    appendToBlock(b, c);
    return true;
}

bool parseExpression(Lexer *l, Expression **e) {
    Expression *left = NULL;
    if (!parseExpressionCmp(l, &left)) {
        return false;
    }
    Token *t = peekToken(l);
    if (t->type != SYMBOL && (t->val.symbol != SYM_EQUALS ||
                              t->val.symbol != SYM_NOT_EQUALS)) {
        *e = left;
        return true;
    }
    nextToken(l);
    Expression *right = NULL;
    if (!parseExpression(l, &right)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    *e = createExpression(E_BINARY);
    (*e)->data.binary.op
        = t->val.symbol == SYM_EQUALS ? EB_EQUAL : EB_NOT_EQUAL;
    (*e)->data.binary.left = left;
    (*e)->data.binary.right = right;
    return true;
}

bool parseExpressionCmp(Lexer *l, Expression **e) {
    Expression *left = NULL;
    if (!parseExpressionMul(l, &left)) {
        return false;
    }
    Token *t = peekToken(l);
    if (t->type != SYMBOL && (t->val.symbol != SYM_LESS ||
                              t->val.symbol != SYM_LESS_EQUAL ||
                              t->val.symbol != SYM_GREATER ||
                              t->val.symbol != SYM_GREATER_EQUAL)) {
        *e = left;
        return true;
    }
    nextToken(l);
    Expression *right = NULL;
    if (!parseExpression(l, &right)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    *e = createExpression(E_BINARY);
    (*e)->data.binary.op =
        t->val.symbol == SYM_LESS ? EB_LESS :
        t->val.symbol == SYM_LESS_EQUAL ? EB_LESS_EQUAL :
        t->val.symbol == SYM_GREATER ? EB_GREATER : EB_GREATER_EQUAL;
    (*e)->data.binary.left = left;
    (*e)->data.binary.right = right;
    return true;
}

bool parseExpressionMul(Lexer *l, Expression **e) {
    Expression *left = NULL;
    if (!parseExpressionAdd(l, &left)) {
        return false;
    }
    Token *t = peekToken(l);
    if (t->type != SYMBOL && (t->val.symbol != SYM_STAR ||
                              t->val.symbol != SYM_SLASH)) {
        *e = left;
        return true;
    }
    nextToken(l);
    Expression *right = NULL;
    if (!parseExpression(l, &right)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    *e = createExpression(E_BINARY);
    (*e)->data.binary.op
        = t->val.symbol == SYM_STAR ? EB_MULTIPLY : EB_DIVIDE;
    (*e)->data.binary.left = left;
    (*e)->data.binary.right = right;
    return true;
}

bool parseExpressionAdd(Lexer *l, Expression **e) {
    Expression *left = NULL;
    if (!parseExpressionTerm(l, &left)) {
        return false;
    }
    Token *t = peekToken(l);
    if (t->type != SYMBOL && (t->val.symbol != SYM_PLUS ||
                              t->val.symbol != SYM_MINUS)) {
        *e = left;
        return true;
    }
    nextToken(l);
    Expression *right = NULL;
    if (!parseExpression(l, &right)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    *e = createExpression(E_BINARY);
    (*e)->data.binary.op = t->val.symbol == SYM_PLUS ? EB_ADD : EB_SUBTRACT;
    (*e)->data.binary.left = left;
    (*e)->data.binary.right = right;
    return true;
}

bool parseExpressionTerm(Lexer *l, Expression **e) {
    bracket(l, parseExpressionParen, e);
    bracket(l, parseExpressionFuncall, e);
    bracket(l, parseExpressionLiteral, e);
    bracket(l, parseExpressionVar, e);
    return false;
}

bool parseExpressionParen(Lexer *l, Expression **e) {
    trySymbol(l, SYM_PAREN_OPEN, false);
    if (!parseExpression(l, e)) {
        FERROR(ERR_SYNTAX,
               "Expected an expression on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
    expectSymbol(l, SYM_PAREN_CLOSE);
    return true;
}

bool parseExpressionFuncall(Lexer *l, Expression **e) {
    Token *t = peekToken(l);
    if (t->type != ID_SIMPLE && t->type != ID_COMPOUND)
        return false;
    char *name = parseAndQualifyId(l);
    trySymbol(l, SYM_PAREN_OPEN, (free(name), false));
    int argCount = 0;
    Expression *argHead = parseArgListCall(l, &argCount);
    expectSymbol(l, SYM_PAREN_CLOSE);
    *e = createExpression(E_FUNCALL);
    (*e)->data.funcall.name = name;
    (*e)->data.funcall.argCount = argCount;
    (*e)->data.funcall.argHead = argHead;
    return true;
}

bool parseExpressionLiteral(Lexer *l, Expression **e) {
    Token *t = peekToken(l);
    Value *v;
    switch (t->type) {
    case RESERVED:
        if (t->val.reserved == RES_TRUE) {
            v = createValue(T_BOOLEAN);
            v->data.boolean = true;
        } else if (t->val.reserved == RES_FALSE) {
            v = createValue(T_BOOLEAN);
            v->data.boolean = false;
        } else {
            return false;
        }
        break;
    case LIT_INTEGER:
        v = createValue(T_INTEGER);
        v->data.integer = t->val.intVal;
        break;
    case LIT_STRING:
        v = createValue(T_INTEGER);
        v->data.integer = t->val.intVal;
        break;
    case LIT_DOUBLE:
        v = createValue(T_DOUBLE);
        v->data.integer = t->val.doubleVal;
        break;
    default:
        return false;
    }
    *e = createExpression(E_VALUE);
    (*e)->data.value = v;
    nextToken(l);
    return true;
}

bool parseExpressionVar(Lexer *l, Expression **e) {
    Token *t = peekToken(l);
    if (t->type != ID_SIMPLE && t->type != ID_COMPOUND)
        return false;
    char *name = parseAndQualifyId(l);
    *e = createExpression(E_REFERENCE);
    (*e)->data.reference = name;
    return true;
}

bool parseDeclaration(Lexer *l, Declaration **d) {
    tryValueType(l, false);
    ValueType type = parseType(l);
    expectType(l, ID_SIMPLE);
    char *name = parseSimpleId(l);

    if (*d == NULL) {
        *d = createDeclaration(type, name);
    } else {
        (*d)->type = type;
        (*d)->name = name;
    }
    return true;
}

Declaration *parseArgListDecl(Lexer *l, int *argCount) {
    Declaration *d = NULL;
    parseDeclaration(l, &d);
    if (d != NULL) {
        (*argCount)++;
        trySymbol(l, SYM_COMMA, d);
        d->next = parseArgListDecl(l, argCount);
    }
    return d;
}

Expression *parseArgListCall(Lexer *l, int *argCount) {
    Expression *e = NULL;
    if (parseExpression(l, &e)) {
        (*argCount)++;
        trySymbol(l, SYM_COMMA, e);
        e->next = parseArgListCall(l, argCount);
    }
    return e;
}

char *parseSimpleId(Lexer *l) {
    expectType(l, ID_SIMPLE);
    char *orig = getToken(l)->val.id;
    char *new = malloc(strlen(orig) + 1);
    CHECK_ALLOC(new);
    strcpy(new, orig);
    return new;
}

char *parseAndQualifyId(Lexer *l) {
    Token *t = peekToken(l);
    if (t->type == ID_COMPOUND) {
        char *orig = getToken(l)->val.id;
        char *new = malloc(strlen(orig) + 1);
        CHECK_ALLOC(new);
        strcpy(new, orig);
        return new;
    }
    if (t->type == ID_SIMPLE) {
        int classLength = strlen(l->lastClassName);
        int idLength = strlen(t->val.id);
        char *c = malloc((classLength + 1 + idLength + 1));
        CHECK_ALLOC(c);
        strcpy(c, l->lastClassName);
        c[classLength] = '.';
        strcpy(c + classLength + 1, t->val.id);
        c[classLength + 1 + idLength] = '\0';
        return c;
    }
    FERROR(ERR_SYNTAX,
           "Expected an identifier on line %d, char %d.\n",
           peekToken(l)->lineNum, peekToken(l)->lineChar);
}

ValueType parseType(Lexer *l) {
    expectType(l, RESERVED);
    Token *t = getToken(l);
    switch (t->val.reserved) {
    case RES_BOOLEAN: return T_BOOLEAN;
    case RES_DOUBLE: return T_DOUBLE;
    case RES_INT: return T_INTEGER;
    case RES_STRING: return T_STRING;
    default:
        FERROR(ERR_SYNTAX,
               "Expected a type on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
}

ValueType parseReturnType(Lexer *l) {
    expectType(l, RESERVED);
    Token *t = getToken(l);
    switch (t->val.reserved) {
    case RES_BOOLEAN: return T_BOOLEAN;
    case RES_DOUBLE: return T_DOUBLE;
    case RES_INT: return T_INTEGER;
    case RES_STRING: return T_STRING;
    case RES_VOID: return T_VOID;
    default:
        FERROR(ERR_SYNTAX,
               "Expected a return type on line %d, char %d.\n",
               peekToken(l)->lineNum, peekToken(l)->lineChar);
    }
}
