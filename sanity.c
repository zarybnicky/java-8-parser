/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "sanity.h"

static SymbolTable *symTable;
static SymbolTable *localTable;
static bool hasReturnCommand;
static char *className;

void runSemanticAnalysis(SymbolTable *s) {
    symTable = s;
    Node *root = symTable->root;

    checkMainRunPresence();
    table_iterate_fn(root, checkReturnPresence);
    table_iterate_fn(root, checkTopLevel);
    table_iterate_fn(root, checkOperatorAssignmentType);

    symTable = NULL;
}

void freeSemantic() {
    if (localTable != NULL) {
        freeSymbolTable(localTable);
    }
    if (className != NULL) {
        free(className);
    }
}

//----- Check return type -----
void checkReturnPresenceC(Function *f, Command *c) {
    (void) f;
    if (c->type == C_RETURN) {
        hasReturnCommand = true;
    }
}
void checkReturnPresence(Function *f) {
    hasReturnCommand = false;

    traverseCommands(f, checkReturnPresenceC, NULL);
    if (f->returnType != T_VOID && !hasReturnCommand) {
        freeSemantic();
        fprintf(stderr, "In function %s:\n", f->name);
        MERROR(ERR_RUNTIME_UNINITIALIZED, "No 'return' in a non-void function.");
    }
}

void checkMainRunPresence() {
    Node *tmp = table_lookup(symTable, "Main");
    if (tmp == NULL || tmp->type != N_CLASS) {
        freeSemantic();
        MERROR(ERR_SEM_UNDEFINED, "Missing class Main.");
    }
    tmp = table_lookup(symTable, "Main.run");
    if (tmp == NULL || tmp->type != N_FUNCTION) {
        freeSemantic();
        MERROR(ERR_SEM_UNDEFINED, "Missing run method in Main.");
    }
    if (tmp->data.function->returnType != T_VOID){
        freeSemantic();
        MERROR(ERR_SEM_UNDEFINED, "Run is not void function.");
    }
}

void checkOperatorAssignmentTypeC(Function *f, Command *c) {
    if (c == NULL || f == NULL)
        return;

    Node *n;
    ValueType ltype, rtype;

    switch (c->type) {
    case C_DECLARE:
        table_insert_dummy(localTable, c->data.declare);
        break;

    case C_DEFINE:
        ltype = c->data.define.declaration.type;
        rtype = getExpressionType(c->data.define.expr);
        if (!isAssignCompatible(ltype, rtype)) {
            freeSemantic();
            FERROR(ERR_SEM_TYPECHECK, "Cannot assign a(n) %s to %s.\n",
                   showValueType(rtype), showValueType(ltype));
        }
        table_insert_dummy(localTable, c->data.define.declaration);
        break;

    case C_ASSIGN:
        n = table_lookup_either(symTable, localTable, className,
                                c->data.assign.name);
        if (n == NULL) {
            freeSemantic();
            fprintf(stderr, "In function %s:\n", f->name);
            FERROR(ERR_SEM_UNDEFINED,
                   "Trying to assign to an undefined variable '%s'",
                   c->data.assign.name);
        }
        if (n->type != N_VALUE) {
            freeSemantic();
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_SEM_TYPECHECK, "Can't assign to a function");
        }
        ltype = n->data.value->type;
        rtype = getExpressionType(c->data.assign.expr);
        if (!isAssignCompatible(ltype, rtype)) {
            freeSemantic();
            FERROR(ERR_SEM_TYPECHECK, "Cannot assign a(n) %s to %s.\n",
                   showValueType(rtype), showValueType(ltype));
        }
        break;

    case C_IF:
        rtype = getExpressionType(c->data.ifC.cond);
        if (!isAssignCompatible(T_BOOLEAN, rtype)) {
            freeSemantic();
            MERROR(ERR_SEM_TYPECHECK,
                   "'if' condition requires a boolean expression\n");
        }
        break;

    case C_WHILE:
        rtype = getExpressionType(c->data.whileC.cond);
        if (!isAssignCompatible(T_BOOLEAN, rtype)) {
            freeSymbolTable(localTable);
            MERROR(ERR_SEM_TYPECHECK,
                   "'while' condition requires a boolean expression\n");
        }
        break;

    case C_DO_WHILE:
        rtype = getExpressionType(c->data.doWhileC.cond);
        if (!isAssignCompatible(T_BOOLEAN, rtype)) {
            freeSemantic();
            MERROR(ERR_SEM_TYPECHECK,
                   "'do-while' condition requires a boolean expression\n");
        }
        break;

    case C_FOR:
        if (c->data.forC.initial != NULL) {
            ltype = c->data.forC.var.type;
            rtype = getExpressionType(c->data.forC.initial);
            if (!isAssignCompatible(ltype, rtype)) {
                freeSemantic();
                FERROR(ERR_SEM_TYPECHECK,
                       "Can't assign %s to %s in 'for' initialization",
                       showValueType(ltype), showValueType(rtype));
            }
        }
        table_insert_dummy(localTable, c->data.forC.var);

        rtype = getExpressionType(c->data.forC.cond);
        if (!isAssignCompatible(T_BOOLEAN, rtype)) {
            freeSemantic();
            MERROR(ERR_SEM_TYPECHECK,
                   "'for' condition requires a boolean expression\n");
        }
        break;

    case C_EXPRESSION:
        getExpressionType(c->data.expr); //check for undefineds etc.
        break;

    case C_RETURN:
        if (f->returnType == T_VOID) {
            if (c->data.expr != NULL) {
                freeSemantic();
                fprintf(stderr, "In function %s:\n", f->name);
                MERROR(ERR_SEM_TYPECHECK,
                       "Returning a value from a void function");
            }
        } else {
            if (c->data.expr == NULL) {
                freeSemantic();
                fprintf(stderr, "In function %s:\n", f->name);
                MERROR(ERR_SEM_TYPECHECK,
                       "Not returning a value from a non-void function");
            }
            ltype = f->returnType;
            rtype = getExpressionType(c->data.expr);
            if (!isAssignCompatible(ltype, rtype)) {
                freeSemantic();
                FERROR(ERR_SEM_TYPECHECK,
                       "Can't return a %s from a function returning %s\n",
                       showValueType(rtype), showValueType(ltype));
            }
        }
        break;

    case C_BLOCK:
    case C_CONTINUE:
    case C_BREAK:
        break;
    }
}
void checkOperatorAssignmentTypeF(Command *c) {
    freeNode(table_remove(localTable, c->data.forC.var.name));
}
void checkOperatorAssignmentType(Function *f) {
    className = getClassName(f->name);
    localTable = createSymbolTable();

    Declaration *arg = f->argHead;
    while (arg != NULL) {
        table_insert_dummy(localTable, *arg);
        arg = arg->next;
    }
    traverseCommands(f, checkOperatorAssignmentTypeC,
                     checkOperatorAssignmentTypeF);
    freeSemantic();
}

void checkTopLevelInner(Function *f, Command *c, bool cycle) {
    while (c != NULL) {
        switch (c->type) {
        case C_DECLARE:
            freeSemantic();
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_INTERNAL, "C_DECLARE located in a nested block.");
            break;
        case C_DEFINE:
            freeSemantic();
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_INTERNAL, "C_DEFINE located in a nested block.");
            break;
        case C_CONTINUE:
            if (cycle == false) {
                freeSemantic();
                fprintf(stderr, "In function %s:\n", f->name);
                MERROR(ERR_INTERNAL, "C_CONTINUE located outside of a cycle.");
            }
            break;
        case C_BREAK:
            if (cycle == false) {
                freeSemantic();
                fprintf(stderr, "In function %s:\n", f->name);
                MERROR(ERR_INTERNAL, "C_BREAK located outside of a cycle.");
            }
            break;
        case C_IF:
            checkTopLevelInner(f, c->data.ifC.thenBlock.head, cycle);
            checkTopLevelInner(f, c->data.ifC.elseBlock.head, cycle);
            break;
        case C_WHILE:
            checkTopLevelInner(f, c->data.whileC.bodyBlock.head, true);
            break;
        case C_DO_WHILE:
            checkTopLevelInner(f, c->data.doWhileC.bodyBlock.head, true);
            break;
        case C_FOR:
            checkTopLevelInner(f, c->data.forC.iter, false);
            checkTopLevelInner(f, c->data.forC.bodyBlock.head, true);
            break;
        case C_BLOCK:
            checkTopLevelInner(f, c->data.block.head, cycle);
            break;
        case C_ASSIGN:
        case C_EXPRESSION:
        case C_RETURN:
            break;
        }
        c = c->next;
    }
}

// Aplikace v pruchodu stromu
void checkTopLevel(Function *f) {
    for (Command *c = f->body.head; c != NULL; c = c->next) {
        switch (c->type) {
        case C_CONTINUE:
            freeSemantic();
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_INTERNAL, "C_CONTINUE located outside of a cycle.");
            break;
        case C_BREAK:
            freeSemantic();
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_INTERNAL, "C_DEFINE located outside of a cycle.");
            break;
        case C_IF:
            checkTopLevelInner(f, c->data.ifC.thenBlock.head, false);
            checkTopLevelInner(f, c->data.ifC.elseBlock.head, false);
            break;
        case C_WHILE:
            checkTopLevelInner(f, c->data.whileC.bodyBlock.head, true);
            break;
        case C_DO_WHILE:
            checkTopLevelInner(f, c->data.doWhileC.bodyBlock.head, true);
            break;
        case C_FOR:
            checkTopLevelInner(f, c->data.forC.iter, false);
            checkTopLevelInner(f, c->data.forC.bodyBlock.head, true);
            break;
        case C_BLOCK:
            checkTopLevelInner(f, c->data.block.head, false);
            break;
        default:
            break;
        }
    }
}

ValueType getExpressionType(Expression *e) {
    if (e == NULL) {
        freeSemantic();
        MERROR(ERR_INTERNAL, "NULL expression");
    }
    Node *n;
    Expression *arg;
    ValueType t;

    switch (e->type) {
    case E_VALUE:
        return e->data.value->type;

    case E_FUNCALL:
        n = table_lookup_either(symTable, NULL, className, e->data.funcall.name);
        if (n == NULL) {
            freeSemantic();
            FERROR(ERR_SEM_UNDEFINED,
                   "Trying to call a non-existent function %s",
                   e->data.reference);
        }
        if (n->type != N_FUNCTION) {
            freeSemantic();
            FERROR(ERR_SEM_TYPECHECK, "Trying to call a variable %s",
                   e->data.funcall.name);
        }
        if (e->data.funcall.argCount != n->data.function->argCount) {
            freeSemantic();
            FERROR(ERR_SEM_TYPECHECK,
                   "Calling %s with %d arguments, expecting %d",
                   e->data.funcall.name, e->data.funcall.argCount,
                   n->data.function->argCount);
        }

        arg = e->data.funcall.argHead;
        #ifdef DEBUG
        printf("\n");
        printExpression(arg);
        printf("\n");
        printSymbolTable(localTable);
        printSymbolTable(symTable);
        printFunction(n->data.function);
        printf("\n");
        #endif
        for (int argNum = 0; arg != NULL; argNum++, arg = arg->next) {
            Declaration *d = n->data.function->argHead;
            for (int i = n->data.function->argCount - 1; i --> argNum;)
                d = d->next;
            ValueType rtype = getExpressionType(arg);
            if (!isAssignCompatible(d->type, rtype) && strcmp("ifj16.print", e->data.funcall.name) != 0) {
                fprintf(stderr,
                       "Cannot convert %s to %s while calling function %s\n",
                       showValueType(rtype), showValueType(d->type),
                       n->data.function->name);
                freeSemantic();
                ERROR(ERR_SEM_TYPECHECK);
            }
        }
        return n->data.function->returnType;

    case E_REFERENCE:
        n = table_lookup_either(symTable, localTable, className,
                                e->data.reference);
        if (n == NULL) {
            freeSemantic();
            FERROR(ERR_SEM_UNDEFINED,
                   "Trying to reference a non-existent variable %s",
                   e->data.reference);
        }
        if (n->type != N_VALUE) {
            freeSemantic();
            MERROR(ERR_SEM_TYPECHECK, "Trying to reference a function");
        }
        return n->data.value->type;

    case E_BINARY:
        t = getBinExpType(e->data.binary.op,
                          getExpressionType(e->data.binary.left),
                          getExpressionType(e->data.binary.right));
        if (t == T_VOID) {
            fprintf(stderr,"Wrong operator types for operation %s: %s, %s.\n",
                   showBinaryOperation(e->data.binary.op),
                   showValueType(getExpressionType(e->data.binary.left)),
                   showValueType(getExpressionType(e->data.binary.right)));
            freeSemantic();
            ERROR(ERR_SEM_TYPECHECK);
        }
        return t;
    }

    //...and appease the compiler
    return T_VOID;
}
