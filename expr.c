/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "expr.h"

PredStack *createPStack() {
    PredStack *s = malloc_c(sizeof(PredStack));
    CHECK_ALLOC(s);
    s->sStack = malloc_c(20 * sizeof(PredSymbol));
    CHECK_ALLOC(s->sStack);
    s->tStack = malloc_c(20 * sizeof(Token *));
    CHECK_ALLOC(s->tStack);
    s->stackSize = 20;
    s->stackPtr = 0;
    return s;
}
bool emptyPStack(PredStack *s) {
    return s->stackPtr == 0;
}
void pushPStack(PredStack *x, PredSymbol s, void *t) {
    if (x->stackPtr == x->stackSize) {
        x->stackSize *= 2;
        x->sStack = realloc(x->sStack, x->stackSize * sizeof(PredSymbol));
        CHECK_ALLOC(x->sStack);
        x->tStack = realloc(x->tStack, x->stackSize * sizeof(void *));
        CHECK_ALLOC(x->tStack);
    }
    x->sStack[x->stackPtr] = s;
    x->tStack[x->stackPtr] = t;
    x->stackPtr++;
}
void markTerminal(PredStack *s) {
    if (s->stackPtr == s->stackSize) {
        s->stackSize *= 2;
        s->sStack = realloc(s->sStack, s->stackSize * sizeof(PredSymbol));
        CHECK_ALLOC(s->sStack);
        s->tStack = realloc(s->tStack, s->stackSize * sizeof(void *));
        CHECK_ALLOC(s->tStack);
    }
    //Find out where to place the mark...
    int mark = -1;
    for (int i = s->stackPtr; i > 0; i--) {
        if (s->sStack[i - 1] <= P_END) { //non-terminal
            mark = i - 1;
            break;
        }
    }
    if (mark == -1) {
        ERROR(ERR_INTERNAL);
    }
    //...and do it.
    for (int i = s->stackPtr; i > mark; i--) {
        s->sStack[i] = s->sStack[i - 1];
        s->tStack[i] = s->tStack[i - 1];
    }
    s->sStack[mark + 1] = P_MARK;
    s->tStack[mark + 1] = NULL;
    s->stackPtr++;
}
Expression *peekNonTerminal(PredStack *s) {
    if (s->stackPtr == 0) {
        ERROR(ERR_INTERNAL);
    }
    for (unsigned i = s->stackPtr; i > 0; i--) {
        if (s->sStack[i - 1] == P_NON) {
            return s->tStack[i - 1];
        }
    }
    return NULL;
}
PredSymbol peekTerminal(PredStack *s, Token **t) {
    if (s->stackPtr == 0) {
        ERROR(ERR_INTERNAL);
    }
    for (unsigned i = s->stackPtr; i > 0; i--) {
        if (s->sStack[i - 1] > P_END) //P_MARK, P_NON(-terminal)
            continue;
        *t = (Token *) s->tStack[i - 1];
        return s->sStack[i - 1];
    }

    *t = NULL;
    return P_END;
}
PredSymbol peekPStack(PredStack *s) {
    if (s->stackPtr == 0) {
        ERROR(ERR_INTERNAL);
    }
    return s->sStack[s->stackPtr - 1];
}
void popPStack(PredStack *s) {
    if (s->stackPtr == 0) {
        ERROR(ERR_INTERNAL);
    }
    s->stackPtr--;
}

Expression *handleSingle(PredStack *s) {
    Expression *e;
    PredSymbol top = s->sStack[s->stackPtr - 1];
    switch (top) {
    case P_NON:
        e = peekNonTerminal(s);
        popPStack(s);
        break;
    default:
        FERROR(ERR_INTERNAL, "handleSingle: invalid symbol %s", showPredSymbol(top));
    }
    return e;
}

void reduce(PredStack *s) {
    Expression *e = NULL;
    Token *t;

    switch (peekTerminal(s, &t)) {
    case P_TF:
    case P_LIT:
    case P_ID:
        e = parseTerm(s->tStack[s->stackPtr - 1]);
        popPStack(s);
        break;
    case P_COMMA:
    case P_LP:
    case P_MARK:
    case P_NON:
        FERROR(ERR_INTERNAL, "Unexpected symbol %s",
               showPredSymbol(peekTerminal(s, &t)));
    case P_RP:
        e = parseFuncallExpr(s);
        break;
    case P_ADD:
    case P_MUL:
    case P_DIV:
    case P_LT:
    case P_GT:
    case P_LE:
    case P_GE:
    case P_EQ:
    case P_NEQ:
    case P_AND:
    case P_OR:
        e = parseBinExpr(s);
        break;
    }

    if (e == NULL) {
        FERROR(ERR_INTERNAL, "As of yet unknown symbol %s",
               showPredSymbol(peekTerminal(s, &t)));
    }

    if (peekPStack(s) != P_MARK) {
        FERROR(ERR_INTERNAL, "Expected P_MARK, instead received %s",
               showPredSymbol(peekPStack(s)));
    }
    popPStack(s);
    pushPStack(s, P_NON, e);
}

Expression *parseExpression(Lexer *l, SymbolType end) {
    Token *t;
    PredStack *s = createPStack();
    pushPStack(s, P_END, NULL);

    while ((translatePred(peekToken(l)) != P_END && !isSymbol(l, end)) || peekTerminal(s, &t) != P_END) {
        PredSymbol a = peekTerminal(s, &t);
        t = peekToken(l);
        PredSymbol b = translatePred(t);

        switch (predTable[a][b]) {
        case O:
            printToken(t);
            FERROR(ERR_SYNTAX, "Invalid token encountered on %d:%d, '%s'",
                   t->lineNum, t->lineChar, t->original);
        case L:
            markTerminal(s);
            pushPStack(s, b, t);
            nextToken(l);
            break;
        case E:
            pushPStack(s, b, t);
            nextToken(l);
            break;
        case G:
            reduce(s);
            break;
        }

        #if DEBUG
        printf("%s%s%s\t", showPredSymbol(a), showPredType(predTable[a][b]), showPredSymbol(b));
        for (unsigned i = 0; i < s->stackPtr; i++) {
            printf("%s ", showPredSymbol(s->sStack[i]));
        }
        putchar('\n');
        #endif
    }

    #if DEBUG
    putchar('\n');
    #endif
    Expression *e = peekNonTerminal(s);
    return e;
}

Expression *parseTerm(Token *t) {
    Expression *e;
    switch (t->type) {
    case RESERVED:
        e = createExpression(E_VALUE);
        e->data.value = createValue(T_BOOLEAN);
        if (t->val.reserved == RES_TRUE) {
            e->data.value->data.boolean = true;
        } else if (t->val.reserved == RES_FALSE) {
            e->data.value->data.boolean = false;
        } else {
            FERROR(ERR_SYNTAX, "Unexpected reserved word on line %d:%d '%s'",
                   t->lineNum, t->lineChar, t->original);
        }
        break;
    case LIT_INTEGER:
        e = createExpression(E_VALUE);
        e->data.value = createValue(T_INTEGER);
        e->data.value->data.integer = t->val.intVal;
        break;
    case LIT_STRING:
        e = createExpression(E_VALUE);
        e->data.value = createValue(T_STRING);
        e->data.value->data.str = strdup_(t->val.stringVal);
        break;
    case LIT_DOUBLE:
        e = createExpression(E_VALUE);
        e->data.value = createValue(T_DOUBLE);
        e->data.value->data.dbl = t->val.doubleVal;
        break;
    case ID_SIMPLE:
    case ID_COMPOUND:
        e = createExpression(E_REFERENCE);
        e->data.reference = strdup_(t->val.id);
        break;
    default:
        FERROR(ERR_SYNTAX, "Unexpected token on line %d:%d '%s'",
               t->lineNum, t->lineChar, t->original);
    }
    return e;
}

Expression *parseFuncallExpr(PredStack *s) {
    Token *t;
    PredFunState state = F_START;
    Expression *argHead = NULL, *argNext, *f;
    char *name = NULL;
    int argCount = 0;

    while (peekPStack(s) != P_MARK) {
        switch (state) {
        case F_START:
            switch (peekPStack(s)) {
            case P_RP:
                state = F_RP;
                popPStack(s);
                break;
            default:
                peekTerminal(s, &t);
                FERROR(ERR_SYNTAX, "Unexpected token on line %d:%d '%s'",
                       t->lineNum, t->lineChar, t->original);
            }
            break;
        case F_LP:
            switch (peekPStack(s)) {
            case P_ID:
                peekTerminal(s, &t);
                name = strdup_(t->val.id);
                state = F_END;
                popPStack(s);
                break;
            default:
                peekTerminal(s, &t);
                FERROR(ERR_SYNTAX, "Expected an id on line %d:%d, instead received '%s'",
                       t->lineNum, t->lineChar, t->original);
            }
            break;
        case F_RP:
            switch (peekPStack(s)) {
            case P_LP:
                state = F_LP;
                popPStack(s);
                continue;
            case P_NON:
                if (argHead == NULL) {
                    argHead = peekNonTerminal(s);
                } else {
                    argNext = peekNonTerminal(s);
                    argNext->next = argHead;
                    argHead = argNext;
                }
                argCount++;
                state = F_E;
                popPStack(s);
                break;
            default:
                peekTerminal(s, &t);
                FERROR(ERR_SYNTAX, "Expected an expression on line %d:%d, instead received '%s'",
                       t->lineNum, t->lineChar, t->original);
            }
            break;
        case F_E:
            switch (peekPStack(s)) {
            case P_COMMA:
                state = F_C;
                popPStack(s);
                break;
            case P_LP:
                state = F_LP;
                popPStack(s);
                break;
            default:
                peekTerminal(s, &t);
                FERROR(ERR_SYNTAX, "Unexpected symbol on line %d:%d '%s'",
                       t->lineNum, t->lineChar, t->original);
            }
            break;
        case F_C:
            switch (peekPStack(s)) {
            case P_NON:
                if (argHead == NULL) {
                    argHead = peekNonTerminal(s);
                } else {
                    argNext = peekNonTerminal(s);
                    argNext->next = argHead;
                    argHead = argNext;
                }
                argCount++;
                state = F_E;
                popPStack(s);
                break;
            default:
                peekTerminal(s, &t);
                FERROR(ERR_SYNTAX, "Unexpected symbol on line %d:%d '%s'",
                       t->lineNum, t->lineChar, t->original);
            }
            break;
        case F_END:
            ERROR(ERR_INTERNAL);
        }
    }
    if (name == NULL) {
        if (argCount != 1) {
            peekTerminal(s, &t);
            FERROR(ERR_SYNTAX, "Invalid bracketed expression on line %d:%d",
                   t->lineNum, t->lineChar);
        }
        return argHead; //an expression, not a function call
    }
    f = createExpression(E_FUNCALL);
    f->data.funcall.name = name;
    f->data.funcall.argCount = argCount;
    f->data.funcall.argHead = argHead;
    return f;
}

BinaryOperation translateBinOp(Token *t) {
    if (t->type != SYMBOL) {
        ERROR(ERR_INTERNAL);
    }
    switch (t->val.symbol) {
    case SYM_PLUS:          return EB_ADD;
    case SYM_MINUS:         return EB_SUBTRACT;
    case SYM_STAR:          return EB_MULTIPLY;
    case SYM_SLASH:         return EB_DIVIDE;
    case SYM_LESS:          return EB_LESS;
    case SYM_GREATER:       return EB_GREATER;
    case SYM_LESS_EQUAL:    return EB_LESS_EQUAL;
    case SYM_GREATER_EQUAL: return EB_GREATER_EQUAL;
    case SYM_EQUALS:        return EB_EQUAL;
    case SYM_NOT_EQUALS:    return EB_NOT_EQUAL;
    case SYM_AND:           return EB_AND;
    case SYM_OR:            return EB_OR;
    default:
        ERROR(ERR_INTERNAL);
    }
}

Expression *parseBinExpr(PredStack *s) {
    Expression *a, *b;
    BinaryOperation op;
    Token *t;

    if (peekPStack(s) != P_NON) {
        peekTerminal(s, &t);
        FERROR(ERR_SYNTAX, "Unexpected symbol on line %d:%d '%s'",
               t->lineNum, t->lineChar, t->original);
    }
    a = peekNonTerminal(s);
    popPStack(s);
    peekTerminal(s, &t);
    if (peekPStack(s) >= P_END) {
        FERROR(ERR_SYNTAX, "Unexpected symbol on line %d:%d '%s'",
               t->lineNum, t->lineChar, t->original);
    }
    op = translateBinOp(t);
    popPStack(s);
    if (peekPStack(s) != P_NON) {
        peekTerminal(s, &t);
        FERROR(ERR_SYNTAX, "Unexpected symbol on line %d:%d '%s'",
               t->lineNum, t->lineChar, t->original);
    }
    b = peekNonTerminal(s);
    popPStack(s);

    Expression *e = createExpression(E_BINARY);
    e->data.binary.op = op;
    e->data.binary.left = a;
    e->data.binary.right = b;
    return e;
}
