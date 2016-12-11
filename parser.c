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
    Lexer *l = malloc_c(sizeof(Lexer));
    l->file = f;
    l->start = l->current = NULL;
    l->stackSize = 20;
    l->stackPtr = 0;
    l->stack = malloc_c(l->stackSize * sizeof(Token *));
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
        free_c(l->stack);
        free_c(l);
    }
}

bool try(Lexer *l) {
    if (l->stackPtr == l->stackSize) {
        l->stackSize *= 2;
        l->stack = realloc_c(l->stack, l->stackSize * sizeof(Token *));
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
    expectSymbol(l, SYM_BRACE_OPEN);

    table_insert(&l->interpret->symTable, createClassNode(name));
    l->lastClassName = name;
    while (parseClassBody(l))
        ;
    expectSymbol(l, SYM_BRACE_CLOSE);
    commit(l);
}

bool parseClassBody(Lexer *l) {
    if (isSymbol(l, SYM_BRACE_CLOSE)) {
        return false;
    }

    expectReserved(l, RES_STATIC);
    ValueType type = parseReturnType(l);
    char *name = parseAndQualifySimpleId(l);

    if (isSymbol(l, SYM_SEMI)) {
        nextToken(l);
        Value *v = createValue(type);
        v->undefined = true;
        table_insert(&l->interpret->symTable, createValueNode(name, v));
        return true;
    }

    if (isSymbol(l, SYM_ASSIGN)) {
        nextToken(l);
        PARSE_EXPRESSION(e, l, SYM_SEMI);

        Value *v = evalStaticExpression(e);
        free_c(e);
        if (v->type != type) {
            FERROR(ERR_SEM_TYPECHECK,
                   "Type error: variable has type %s but assigned expression has evaluated to %s\n",
                   showValueType(type), showValueType(type));
        }
        expectSymbol(l, SYM_SEMI);
        table_insert(&l->interpret->symTable, createValueNode(name, v));
        return true;
    }

    if (isSymbol(l, SYM_PAREN_OPEN)) {
        nextToken(l);
        int argCount = 0;
        Declaration *argList = parseArgListDecl(l, &argCount);
        expectSymbol(l, SYM_PAREN_CLOSE);
        expectSymbol(l, SYM_BRACE_OPEN);

        Function *f = createFunction(name, type, argCount, argList);
        while (parseFunctionBody(l, &f->body))
            ;
        expectSymbol(l, SYM_BRACE_CLOSE);

        table_insert(&l->interpret->symTable, createFunctionNode(strdup_(name), f));
        return true;
    }

    return false;
}

bool parseFunctionBody(Lexer *l, Block *b) {
    if (isSymbol(l, SYM_BRACE_CLOSE)) {
        return false;
    }

    Declaration d, *dPtr = &d;
    if (parseDeclaration(l, &dPtr)) {
        if (isSymbol(l, SYM_SEMI)) {
            nextToken(l);
            appendToBlock(b, createCommandDeclare(d));
            return true;
        }
        if (isSymbol(l, SYM_ASSIGN)) {
            nextToken(l);
            PARSE_EXPRESSION(e, l, SYM_SEMI);
            expectSymbol(l, SYM_SEMI);
            appendToBlock(b, createCommandDefine(d, e));
            return true;
        }
    }

    return parseCommand(l, b);
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
    PARSE_EXPRESSION(e, l, SYM_PAREN_CLOSE);
    expectSymbol(l, SYM_PAREN_CLOSE);

    Command *c = createCommandIf(e);
    if (!parseCommand(l, &c->data.ifC.thenBlock)) {
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
    PARSE_EXPRESSION(e, l, SYM_PAREN_CLOSE);
    expectSymbol(l, SYM_PAREN_CLOSE);

    Command *c = createCommandWhile(e);
    if (!parseCommand(l, &c->data.whileC.bodyBlock)) {
        errorExpectedCommand(l);
    }
    appendToBlock(b, c);
    return true;
}

bool parseDoWhile(Lexer *l, Block *b) {
    tryReserved(l, RES_DO, false);
    Command *c = createCommandDoWhile();
    if (!parseCommand(l, &c->data.doWhileC.bodyBlock)) {
        errorExpectedCommand(l);
    }

    expectReserved(l, RES_WHILE);
    expectSymbol(l, SYM_PAREN_OPEN);
    PARSE_EXPRESSION(e, l, SYM_PAREN_CLOSE);
    c->data.doWhileC.cond = e;
    expectSymbol(l, SYM_PAREN_CLOSE);
    expectSymbol(l, SYM_SEMI);

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
        PARSE_EXPRESSION(init_, l, SYM_SEMI);
        init = init_;
    } else {
        init = NULL;
    }
    expectSymbol(l, SYM_SEMI);
    PARSE_EXPRESSION(cond, l, SYM_SEMI);
    expectSymbol(l, SYM_SEMI);
    Block dummy; dummy.head = dummy.tail = NULL;
    if (!parseAssignExpression(l, &dummy, SYM_PAREN_CLOSE)) {
        errorExpectedCommand(l);
    }
    expectSymbol(l, SYM_PAREN_CLOSE);

    Command *c = createCommandFor(d, init, cond, dummy.head);
    if (!parseCommand(l, &c->data.forC.bodyBlock)) {
        errorExpectedCommand(l);
    }
    appendToBlock(b, c);
    return true;
}

bool parseAssign(Lexer *l, Block *b) {
    if (!parseAssignExpression(l, b, SYM_SEMI))
        return false;
    expectSymbol(l, SYM_SEMI);
    return true;
}

bool parseAssignExpression(Lexer *l, Block *b, SymbolType end) {
    Token *t = peekToken(l);
    if (t->type != ID_SIMPLE && t->type != ID_COMPOUND)
        return false;
    char *name = parseAnyId(l);
    trySymbol(l, SYM_ASSIGN, (free_c(name), false));
    PARSE_EXPRESSION(e, l, end);
    appendToBlock(b, createCommandAssign(name, e));
    return true;
}

bool parseFuncall(Lexer *l, Block *b) {
    Expression *e = parseExpression(l, SYM_SEMI);
    if (e == NULL) {
        return false;
    }
    expectSymbol(l, SYM_SEMI);
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
        nextToken(l);
        appendToBlock(b, createCommandReturn(NULL));
        return true;
    }
    PARSE_EXPRESSION(e, l, SYM_SEMI);
    expectSymbol(l, SYM_SEMI);
    appendToBlock(b, createCommandReturn(e));
    return true;
}

bool parseBlock(Lexer *l, Block *b) {
    trySymbol(l, SYM_BRACE_OPEN, false);
    Command *c = createCommandBlock();
    while (parseFunctionBody(l, &c->data.block))
        ;
    expectSymbol(l, SYM_BRACE_CLOSE);
    appendToBlock(b, c);
    return true;
}

bool parseExpressionLiteral(Lexer *l, Expression **e) {
    return l && e;
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

bool parseExpressionParen(Lexer *l, Expression **e) {
    trySymbol(l, SYM_PAREN_OPEN, false);
    PARSE_EXPRESSION(e_, l, SYM_PAREN_CLOSE);
    *e = e_;
    expectSymbol(l, SYM_PAREN_CLOSE);
    return true;
}

bool parseExpressionFuncall(Lexer *l, Expression **e) {
    Token *t = peekToken(l);
    if (t->type != ID_SIMPLE && t->type != ID_COMPOUND)
        return false;
    char *name = parseAnyId(l);
    trySymbol(l, SYM_PAREN_OPEN, (free_c(name), false));
    int argCount = 0;
    Expression *argHead = parseArgListCall(l, &argCount);
    expectSymbol(l, SYM_PAREN_CLOSE);
    *e = createExpression(E_FUNCALL);
    (*e)->data.funcall.name = name;
    (*e)->data.funcall.argCount = argCount;
    (*e)->data.funcall.argHead = argHead;
    return true;
}

bool parseExpressionTerm(Lexer *l, Expression **e) {
    bracket(l, parseExpressionParen, e);
    bracket(l, parseExpressionFuncall, e);
    bracket(l, parseExpressionLiteral, e);
    bracket(l, parseExpressionVar, e);
    return false;
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
    PARSE_EXPRESSION(right, l, SYM_SEMI);

    *e = createExpression(E_BINARY);
    (*e)->data.binary.op = t->val.symbol == SYM_PLUS ? EB_ADD : EB_SUBTRACT;
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
    PARSE_EXPRESSION(right, l, SYM_SEMI);

    *e = createExpression(E_BINARY);
    (*e)->data.binary.op
        = t->val.symbol == SYM_STAR ? EB_MULTIPLY : EB_DIVIDE;
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
    PARSE_EXPRESSION(right, l, SYM_SEMI);

    *e = createExpression(E_BINARY);
    (*e)->data.binary.op =
        t->val.symbol == SYM_LESS ? EB_LESS :
        t->val.symbol == SYM_LESS_EQUAL ? EB_LESS_EQUAL :
        t->val.symbol == SYM_GREATER ? EB_GREATER : EB_GREATER_EQUAL;
    (*e)->data.binary.left = left;
    (*e)->data.binary.right = right;
    return true;
}

bool parseExpression_(Lexer *l, Expression **e) {
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
    PARSE_EXPRESSION(right, l, SYM_SEMI);

    *e = createExpression(E_BINARY);
    (*e)->data.binary.op
        = t->val.symbol == SYM_EQUALS ? EB_EQUAL : EB_NOT_EQUAL;
    (*e)->data.binary.left = left;
    (*e)->data.binary.right = right;
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
        d = NULL;
    }
}

Expression *parseArgListCall(Lexer *l, int *argCount) {
    Expression *e = parseExpression(l, SYM_SEMI);
    if (e == NULL)
        return NULL;

    (*argCount)++;
    Expression **f = &e->next;
    while (1) {
        trySymbol(l, SYM_COMMA, e);
        PARSE_EXPRESSION(f_, l, SYM_SEMI);
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
    Token *t = peekToken(l);
    int classLength = strlen(l->lastClassName);
    int idLength = strlen(t->val.id);
    char *c = malloc_c((classLength + 1 + idLength + 1));
    strcpy(c, l->lastClassName);
    c[classLength] = '.';
    strcpy(c + classLength + 1, t->val.id);
    c[classLength + 1 + idLength] = '\0';
    nextToken(l);
    return c;
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
