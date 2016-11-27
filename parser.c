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

Lexer *createLexer(FILE *f, Interpret *i) {
    Lexer *l = malloc(sizeof(Lexer));
    CHECK_ALLOC(l);
    l->file = f;
    l->start = l->current = NULL;
    l->stackSize = 20;
    l->stackPtr = 0;
    l->stack = malloc(l->stackSize * sizeof(Token *));
    CHECK_ALLOC_(l->stack, free(l));
    l->interpret = i;
    l->lastClassName = NULL;
    return l;
}
void freeLexer(Lexer *l) {
    if (l != NULL) {
        if (l->file != NULL) {
            fclose(l->file);
        }
        Token *t;
        while (l->start != NULL) {
            t = l->start->next;
            freeToken(l->start);
            l->start = t;
        }
        free(l->stack);
        free(l);
    }
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
        return true;
    }
    while (l->start != l->current) {
        Token *t = l->start;
        l->start = t->next;
        freeToken(t);
    }
    return true;
}

void parseClass(Lexer *l) {
    expectReserved(l, RES_CLASS);
    char *name = parseSimpleId(l);
    expectSymbol_(l, SYM_BRACE_OPEN, free(name));

    table_insert(&l->interpret->symTable, createClassNode(name));
    l->lastClassName = name;
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
    char *name = parseAndQualifySimpleId(l);
    trySymbol(l, SYM_SEMI, (free(name), false));

    Value *v = createValue(type);
    v->undefined = true;
    table_insert(&l->interpret->symTable, createValueNode(name, v));
    return true;
}

bool parseStaticDefinition(Lexer *l) {
    tryReserved(l, RES_STATIC, false);
    ValueType type = parseType(l);
    char *name = parseAndQualifySimpleId(l);
    trySymbol(l, SYM_ASSIGN, (free(name), false));
    PARSE_EXPRESSION(e, l);

    Value *v = evalStaticExpression(e);
    free(e);
    if (v->type != type) {
        free(name);
        free(v);
        FERROR(ERR_SEM_TYPECHECK,
               "Type error: variable has type %s but assigned expression has evaluated to %s\n",
               showValueType(type), showValueType(type));
    }
    expectSymbol_(l, SYM_SEMI, (free(name), freeValue(v)));
    table_insert(&l->interpret->symTable, createValueNode(name, v));
    return true;
}

bool parseFunction(Lexer *l) {
    tryReserved(l, RES_STATIC, false);
    ValueType type = parseReturnType(l);
    char *name = parseAndQualifySimpleId(l);
    trySymbol(l, SYM_PAREN_OPEN, (free(name), false));

    int argCount = 0;
    Declaration *argList = parseArgListDecl(l, &argCount);
    expectSymbol_(l, SYM_PAREN_CLOSE, (free(name), freeDeclaration(argList)));
    expectSymbol_(l, SYM_BRACE_OPEN, (free(name), freeDeclaration(argList)));

    Function *f = createFunction(name, type, argCount, argList);
    while (parseFunctionBody(l, &f->body))
        ;
    expectSymbol_(l, SYM_BRACE_CLOSE, freeFunction(f));

    table_insert(&l->interpret->symTable, createFunctionNode(strdup_(name), f));
    return true;
}

bool parseFunctionBody(Lexer *l, Block *b) {
    bracket(l, parseLocalDeclaration, b);
    bracket(l, parseLocalDefinition, b);
    bracket(l, parseCommand, b);
    return false;
}

bool parseLocalDeclaration(Lexer *l, Block *b) {
    Declaration d, *dPtr = &d;
    if (!parseDeclaration(l, &dPtr)) {
        return false;
    }
    trySymbol(l, SYM_SEMI, (free(d.name), false));
    appendToBlock(b, createCommandDeclare(d));
    return true;
}

bool parseLocalDefinition(Lexer *l, Block *b) {
    Declaration d, *dPtr = &d;
    if (!parseDeclaration(l, &dPtr)) {
        return false;
    }
    expectSymbol_(l, SYM_ASSIGN, free(d.name));
    PARSE_EXPRESSION(e, l);
    expectSymbol_(l, SYM_SEMI, (free(d.name), freeExpression(e)));

    appendToBlock(b, createCommandDefine(d, e));
    return true;
}

bool parseCommand(Lexer *l, Block *b) {
    bracket(l, parseIf, b);
    bracket(l, parseWhile, b);
    bracket(l, parseDoWhile, b);
    bracket(l, parseFor, b);
    bracket(l, parseAssign, b);
    bracket(l, parseFuncall, b);
    bracket(l, parseContinue, b);
    bracket(l, parseBreak, b);
    bracket(l, parseReturn, b);
    bracket(l, parseBlock, b);
    return false;
}

bool parseIf(Lexer *l, Block *b) {
    tryReserved(l, RES_IF, false);

    expectSymbol(l, SYM_PAREN_OPEN);
    PARSE_EXPRESSION(e, l);
    expectSymbol_(l, SYM_PAREN_CLOSE, freeExpression(e));

    Command *c = createCommandIf(e);
    if (!parseCommand(l, &c->data.ifC.thenBlock)) {
        freeCommand(c);
        errorExpectedCommand(l);
    }
    appendToBlock(b, c);
    tryReserved(l, RES_ELSE, true);
    if (!parseCommand(l, &c->data.ifC.elseBlock)) {
        errorExpectedCommand(l);
    }
    return true;
}

bool parseWhile(Lexer *l, Block *b) {
    tryReserved(l, RES_WHILE, false);
    expectSymbol(l, SYM_PAREN_OPEN);
    PARSE_EXPRESSION(e, l);
    expectSymbol_(l, SYM_PAREN_CLOSE, freeExpression(e));

    Command *c = createCommandWhile(e);
    if (!parseCommand(l, &c->data.whileC.bodyBlock)) {
        freeCommand(c);
        errorExpectedCommand(l);
    }
    appendToBlock(b, c);
    return true;
}

bool parseDoWhile(Lexer *l, Block *b) {
    tryReserved(l, RES_DO, false);
    Command *c = createCommandDoWhile();
    if (!parseCommand(l, &c->data.whileC.bodyBlock)) {
        freeCommand(c);
        errorExpectedCommand(l);
    }

    expectReserved_(l, RES_WHILE, freeCommand(c));
    expectSymbol_(l, SYM_PAREN_OPEN, freeCommand(c));
    PARSE_EXPRESSION(e, l);
    c->data.doWhileC.cond = e;
    expectSymbol_(l, SYM_PAREN_CLOSE, freeCommand(c));
    expectSymbol_(l, SYM_SEMI, freeCommand(c));

    appendToBlock(b, c);
    return true;
}

bool parseFor(Lexer *l, Block *b) {
    tryReserved(l, RES_FOR, false);
    expectSymbol(l, SYM_PAREN_OPEN);
    Declaration d, *dPtr = &d;
    if (!parseDeclaration(l, &dPtr)) {
        return false;
    }
    Token *t = peekToken(l);
    Expression *init;
    if (t->type == SYMBOL && t->val.symbol == SYM_ASSIGN) {
        nextToken(l);
        PARSE_EXPRESSION(init_, l);
        init = init_;
    } else {
        init = NULL;
    }
    expectSymbol_(l, SYM_SEMI, (free(d.name)));
    PARSE_EXPRESSION(cond, l);
    expectSymbol_(l, SYM_SEMI, (free(d.name), freeExpression(cond)));
    Block dummy; dummy.head = dummy.tail = NULL;
    if (!parseAssignExpression(l, &dummy)) {
        free(d.name), freeExpression(cond);
        errorExpectedCommand(l);
    }
    expectSymbol_(l, SYM_PAREN_CLOSE, (free(d.name), freeExpression(cond), freeCommand(dummy.head)));

    Command *c = createCommandFor(d, init, cond, dummy.head);
    if (!parseCommand(l, &c->data.whileC.bodyBlock)) {
        freeCommand(c);
        errorExpectedCommand(l);
    }
    appendToBlock(b, c);
    return true;
}

bool parseAssign(Lexer *l, Block *b) {
    if (!parseAssignExpression(l, b))
        return false;
    expectSymbol(l, SYM_SEMI);
    return true;
}

bool parseAssignExpression(Lexer *l, Block *b) {
    Token *t = peekToken(l);
    if (t->type != ID_SIMPLE && t->type != ID_COMPOUND)
        return false;
    char *name = parseAnyId(l);
    trySymbol(l, SYM_ASSIGN, (free(name), false));
    PARSE_EXPRESSION(e, l);
    appendToBlock(b, createCommandAssign(name, e));
    return true;
}

bool parseFuncall(Lexer *l, Block *b) {
    Expression *e = NULL;
    if (!parseExpressionFuncall(l, &e)) {
        return false;
    }
    expectSymbol_(l, SYM_SEMI, freeExpression(e));
    appendToBlock(b, createCommandExpression(e));
    return true;
}

bool parseContinue(Lexer *l, Block *b) {
    tryReserved(l, RES_CONTINUE, false);
    expectSymbol(l, SYM_SEMI);
    appendToBlock(b, createCommandContinue());
    return true;
}

bool parseBreak(Lexer *l, Block *b) {
    tryReserved(l, RES_BREAK, false);
    expectSymbol(l, SYM_SEMI);
    appendToBlock(b, createCommandBreak());
    return true;
}

bool parseReturn(Lexer *l, Block *b) {
    tryReserved(l, RES_RETURN, false);
    if (isSymbol(l, SYM_SEMI)) {
        appendToBlock(b, createCommandReturn(NULL));
        return true;
    }
    PARSE_EXPRESSION(e, l);
    expectSymbol_(l, SYM_SEMI, freeExpression(e));
    appendToBlock(b, createCommandReturn(e));
    return true;
}

bool parseBlock(Lexer *l, Block *b) {
    trySymbol(l, SYM_BRACE_OPEN, false);
    Command *c = createCommandBlock();
    while (parseFunctionBody(l, &c->data.block))
        ;
    expectSymbol_(l, SYM_BRACE_CLOSE, freeCommand(c));
    appendToBlock(b, c);
    return true;
}

bool parseExpression(Lexer *l, Expression **e) {
    Expression *left = NULL;
    if (!parseExpressionCmp(l, &left)) {
        return false;
    }
    Token *t = peekToken(l);
    if (t->type != SYMBOL || (t->val.symbol != SYM_EQUALS &&
                              t->val.symbol != SYM_NOT_EQUALS)) {
        *e = left;
        return true;
    }
    nextToken(l);
    PARSE_EXPRESSION(right, l);

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
    if (t->type != SYMBOL || (t->val.symbol != SYM_LESS &&
                              t->val.symbol != SYM_LESS_EQUAL &&
                              t->val.symbol != SYM_GREATER &&
                              t->val.symbol != SYM_GREATER_EQUAL)) {
        *e = left;
        return true;
    }
    nextToken(l);
    PARSE_EXPRESSION(right, l);

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
    if (t->type != SYMBOL || (t->val.symbol != SYM_STAR &&
                              t->val.symbol != SYM_SLASH)) {
        *e = left;
        return true;
    }
    nextToken(l);
    PARSE_EXPRESSION(right, l);

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
    if (t->type != SYMBOL || (t->val.symbol != SYM_PLUS &&
                              t->val.symbol != SYM_MINUS)) {
        *e = left;
        return true;
    }
    nextToken(l);
    PARSE_EXPRESSION(right, l);

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
    PARSE_EXPRESSION(e_, l);
    *e = e_;
    expectSymbol_(l, SYM_PAREN_CLOSE, freeExpression(e_));
    return true;
}

bool parseExpressionFuncall(Lexer *l, Expression **e) {
    Token *t = peekToken(l);
    if (t->type != ID_SIMPLE && t->type != ID_COMPOUND)
        return false;
    char *name = parseAnyId(l);
    trySymbol(l, SYM_PAREN_OPEN, (free(name), false));
    int argCount = 0;
    Expression *argHead = parseArgListCall(l, &argCount);
    expectSymbol_(l, SYM_PAREN_CLOSE, (free(name), freeExpression(argHead)));
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
        v = createValue(T_STRING);
        v->data.str = strdup_(t->val.stringVal);
        break;
    case LIT_DOUBLE:
        v = createValue(T_DOUBLE);
        v->data.dbl = t->val.doubleVal;
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
    char *name = parseAnyId(l);
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
        (*d)->next = NULL;
    }
    return true;
}

Declaration *parseArgListDecl(Lexer *l, int *argCount) {
    Declaration *d = NULL;
    if (!parseDeclaration(l, &d))
        return NULL;

    (*argCount)++;
    Declaration *head = d;
    d = NULL;
    while (1) {
        trySymbol(l, SYM_COMMA, head);
        if (!parseDeclaration(l, &d)) {
            Token *t = peekToken(l);
            FERROR(ERR_SYNTAX,
                   "Expected a type on line %d:%d, received '%s'.\n",
                   t->lineNum, t->lineChar, t->original);
        }
        (*argCount)++;
        d->next = head;
        head = d;
    }
}

Expression *parseArgListCall(Lexer *l, int *argCount) {
    Expression *e = NULL;
    if (!parseExpression(l, &e))
        return NULL;

    (*argCount)++;
    Expression **f = &e->next;
    while (1) {
        trySymbol(l, SYM_COMMA, e);
        PARSE_EXPRESSION(f_, l);
        *f = f_;
        (*argCount)++;
        f = &(*f)->next;
    }
    return e;
}

char *parseSimpleId(Lexer *l) {
    expectType(l, ID_SIMPLE);
    return strdup_(getToken(l)->val.id);
}
char *parseAndQualifySimpleId(Lexer *l) {
    expectType(l, ID_SIMPLE);
    return parseAndQualifyId(l);
}
char *parseAndQualifyId(Lexer *l) {
    Token *t = peekToken(l);
    if (t->type == ID_COMPOUND) {
        return strdup_(getToken(l)->val.id);
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
        nextToken(l);
        return c;
    }
    FERROR(ERR_SYNTAX,
           "Expected an identifier on line %d:%d, received '%s'.\n",
           t->lineNum, t->lineChar, t->original);
}
char *parseAnyId(Lexer *l) {
    Token *t = peekToken(l);
    if (t->type != ID_COMPOUND && t->type != ID_SIMPLE) {
        FERROR(ERR_SYNTAX,
           "Expected an identifier on line %d:%d, received '%s'.\n",
           t->lineNum, t->lineChar, t->original);
    }
    nextToken(l);
    return strdup_(t->val.id);
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
               "Expected a type on line %d:%d, received '%s'.\n",
               t->lineNum, t->lineChar, t->original);
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
               "Expected a return type on line %d:%d, received '%s'.\n",
               t->lineNum, t->lineChar, t->original);
    }
}
