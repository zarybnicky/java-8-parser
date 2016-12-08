/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "type.h"

ValueType getBinExpType(BinaryOperation op, ValueType left, ValueType right) {
    if (left == T_VOID || right == T_VOID) {
        return T_VOID; //this should never happen
    }
    if (left == T_STRING || right == T_STRING) {
        if (op == EB_ADD) {
            return T_STRING; //concatenation
        }
        return T_VOID;
    }
    if (left == T_BOOLEAN || right == T_BOOLEAN) {
        if (left == T_BOOLEAN && right == T_BOOLEAN &&
            (op == EB_EQUAL || op == EB_NOT_EQUAL)) {
            return T_BOOLEAN; //bool comparison
        }
        return T_VOID;
    }

    //now left & right are both int or double
    switch (op) {
    case EB_ADD:
    case EB_SUBTRACT:
    case EB_MULTIPLY:
    case EB_DIVIDE:
        if (left == T_INTEGER && right == T_INTEGER) {
            return T_INTEGER;
        }
        return T_DOUBLE;
    case EB_EQUAL:
    case EB_NOT_EQUAL:
    case EB_LESS:
    case EB_LESS_EQUAL:
    case EB_GREATER:
    case EB_GREATER_EQUAL:
        return T_BOOLEAN;
    }
    return T_VOID;
}

bool isAssignCompatible(ValueType lvalue, ValueType rvalue) {
    if (lvalue == T_INTEGER && rvalue == T_INTEGER) {
        return true;
    }
    if (lvalue == T_DOUBLE && (rvalue == T_DOUBLE || rvalue == T_INTEGER)) {
        return true;
    }
    if (lvalue == T_STRING && rvalue == T_STRING) {
        return true;
    }
    if (lvalue == T_BOOLEAN && rvalue == T_BOOLEAN) {
        return true;
    }
    return false;
}

Value *coerceTo(ValueType t, Value *v) {
    if (v == NULL || t == v->type || t == T_VOID)
        return v;

    if (t == T_STRING) {
        Value *retVal; size_t bufSize; char *buf;
        switch (v->type) {
        case T_BOOLEAN:
            if (B(v) == true) {
                S(v) = strdup_("true");
            } else {
                S(v) = strdup_("false");
            }
            break;
        case T_INTEGER:
            bufSize = snprintf(NULL, 0, "%d", I(v));
            buf = malloc(bufSize + 1);
            snprintf(buf, bufSize + 1, "%d", I(v));
            retVal=createValue(T_STRING);//malloc_c?
            retVal->data.str = buf;
            return retVal;
        case T_DOUBLE:
            bufSize = snprintf(NULL, 0, "%g", D(v));
            buf = malloc(bufSize + 1);
            snprintf(buf, bufSize + 1, "%g", D(v));
            retVal=createValue(T_STRING); //malloc_c?
            retVal->data.str = buf;
            return retVal;
        default:
            FERROR(ERR_RUNTIME_MISC, "Cannot convert from %s to String",
                   showValueType(v->type));
        }
        v->type = T_STRING;
    }
    if (t == T_INTEGER || t == T_BOOLEAN) {
        //no valid conversions (except for int -> int)
        FERROR(ERR_RUNTIME_MISC, "Cannot convert from %s to int",
               showValueType(v->type));
    }
    if (t == T_DOUBLE) {
        if (v->type == T_INTEGER) {
            D(v) = (double) I(v);
            v->type = T_DOUBLE;
        } else {
            FERROR(ERR_RUNTIME_MISC, "Cannot convert from %s to double",
                   showValueType(v->type));
        }
    }
    return v;
}
