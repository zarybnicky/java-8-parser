/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_EXPR_H
#define IFJ_EXPR_H

#include "scanner.h"
#include "parser.h"
#include <stdbool.h>

typedef enum { O, L, G, E } PredType;

typedef enum { F_START, F_LP, F_RP, F_E, F_C, F_END } PredFunState;

typedef enum {
    P_LP,
    P_RP,
    P_INC,
    P_DEC,
    P_NEG,
    P_MUL,
    P_DIV,
    P_ADD,
    P_SUB,
    P_LT,
    P_GT,
    P_LE,
    P_GE,
    P_EQ,
    P_NEQ,
    P_AND,
    P_OR,
    P_ID,
    P_LIT,
    P_TF,
    P_COMMA,
    P_END,
    P_MARK,
    P_NON,
} PredSymbol;

typedef enum {
    S_START,
    S_VAL,
    S_RP,
    S_RPE,
    S_LP,
    S_END,
} PredState;

typedef struct {
    unsigned stackSize;
    unsigned stackPtr;
    void **tStack;
    PredSymbol *sStack;
} PredStack;

static inline char *showPredType(PredType s) {
    switch (s) {
    case O: return "!";
    case L: return "<";
    case G: return ">";
    case E: return "=";
    }
    return "?"; //..compiler doesn't like enum switches
}

static inline char *showPredSymbol(PredSymbol s) {
    switch (s) {
    case P_RP:   return ")";
    case P_LP:   return "(";
    case P_INC:  return "++";
    case P_DEC:  return "--";
    case P_NEG:  return "!";
    case P_MUL:  return "*";
    case P_DIV:  return "/";
    case P_ADD:  return "+";
    case P_SUB:  return "-";
    case P_LT:   return "<";
    case P_GT:   return ">";
    case P_LE:   return "<=";
    case P_GE:   return ">=";
    case P_EQ:   return "==";
    case P_NEQ:  return "!=";
    case P_AND:  return "&&";
    case P_OR:   return "||";
    case P_ID:   return "I";
    case P_LIT:  return "S";
    case P_TF:   return "B";
    case P_COMMA:return ",";
    case P_END:  return "$";
    case P_MARK: return "#";
    case P_NON:  return "N";
    }
    return "?"; //..compiler doesn't like switches...
}

static inline PredSymbol translatePred(Token *x) {
    switch (x->type) {
    case LIT_INTEGER:
    case LIT_STRING:
    case LIT_DOUBLE:
        return P_LIT;
    case ID_SIMPLE:
    case ID_COMPOUND:
        return P_ID;
    case RESERVED:
        if (x->val.reserved == RES_TRUE || x->val.reserved == RES_FALSE) {
            return P_TF;
        }
        return P_END;
    case SYMBOL:
        switch (x->val.symbol) {
        case SYM_BRACE_OPEN:
        case SYM_BRACE_CLOSE:
        case SYM_BRACKET_OPEN:
        case SYM_BRACKET_CLOSE:
        case SYM_ASSIGN:
        case SYM_SEMI:
            return P_END;
        case SYM_PAREN_OPEN:
            return P_LP;
        case SYM_PAREN_CLOSE:
            return P_RP;
        case SYM_PLUS:
            return P_ADD;
        case SYM_MINUS:
            return P_SUB;
        case SYM_UNARY_PLUS:
            return P_INC;
        case SYM_UNARY_MINUS:
            return P_DEC;
        case SYM_STAR:
            return P_MUL;
        case SYM_SLASH:
            return P_DIV;
        case SYM_COMMA:
            return P_COMMA;
        case SYM_GREATER:
            return P_GT;
        case SYM_GREATER_EQUAL:
            return P_GE;
        case SYM_LESS:
            return P_LT;
        case SYM_LESS_EQUAL:
            return P_LE;
        case SYM_EQUALS:
            return P_EQ;
        case SYM_NOT_EQUALS:
            return P_NEQ;
        case SYM_NOT:
            return P_NEG;
        case SYM_AND:
            return P_AND;
        case SYM_OR:
            return P_OR;
        }
    }
    return P_MARK; //..compiler doesn't like switches...
}

const int predTable[][P_END + 1] = {
    /*(, ),++,--, !, *, /, +, -, <, >,<=,>=,==,!=,&&,||,id,li,tf, ,, $ |*/
    { L, E, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, E, O }, //(
    { O, G, O, O, O, G, G, G, G, G, G, G, G, G, G, G, G, O, O, O, G, G }, //)
    { O, G, G, G, O, G, G, G, G, G, G, G, G, G, G, G, G, L, O, O, G, G }, //++
    { O, G, G, G, O, G, G, G, G, G, G, G, G, G, G, G, G, L, O, O, G, G }, //--
    { L, O, O, O, G, G, G, G, G, G, G, G, G, G, G, G, G, L, O, L, O, O }, //!
    { L, G, L, L, L, G, G, G, G, G, G, G, G, G, G, G, G, L, L, L, G, G }, //*
    { L, G, L, L, L, G, G, G, G, G, G, G, G, G, G, G, G, L, L, L, G, G }, ///
    { L, G, L, L, L, L, L, G, G, G, G, G, G, G, G, G, G, L, L, L, G, G }, //+
    { L, G, L, L, L, L, L, G, G, G, G, G, G, G, G, G, G, L, L, L, G, G }, //-
    { L, G, L, L, L, L, L, L, L, G, G, G, G, G, G, G, G, L, L, L, G, G }, //<
    { L, G, L, L, L, L, L, L, L, G, G, G, G, G, G, G, G, L, L, L, G, G }, //>
    { L, G, L, L, L, L, L, L, L, G, G, G, G, G, G, G, G, L, L, L, G, G }, //<=
    { L, G, L, L, L, L, L, L, L, G, G, G, G, G, G, G, G, L, L, L, G, G }, //>=
    { L, G, L, L, L, L, L, L, L, L, L, L, L, G, G, G, G, L, L, L, G, G }, //==
    { L, G, L, L, L, L, L, L, L, L, L, L, L, G, G, G, G, L, L, L, G, G }, //!=
    { L, G, L, L, L, L, L, L, L, L, L, L, L, L, L, G, G, L, L, L, G, G }, //&&
    { L, G, L, L, L, L, L, L, L, L, L, L, L, L, L, L, G, L, L, L, G, G }, //||
    { E, G, G, G, O, G, G, G, G, G, G, G, G, G, G, G, G, O, O, O, G, G }, //id
    { O, G, O, O, O, G, G, G, G, G, G, G, G, G, G, G, G, O, O, O, G, G }, //lit
    { O, G, O, O, O, G, G, G, G, G, G, G, G, G, G, G, G, O, O, O, G, G }, //t/f
    { L, E, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, E, O }, //,
    { L, O, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, O, O }, //$
};

Expression *parseExpression(Lexer *, SymbolType end);
Expression *parseTerm(Token *);
Expression *parseFuncallExpr(PredStack *);
Expression *parseBinExpr(PredStack *);

#endif /* IFJ_EXPR_H */
