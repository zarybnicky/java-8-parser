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
static bool hasReturnCommand;
static SymbolTable *localTable;
static char *className;
//----- Global variables ------

void runSemanticAnalysis(Interpret *i) {
    symTable = &i->symTable;
    Node *root = symTable->root;

    checkMainRun(&i->symTable);
    table_iterate(root, checkReturnPresence);
    table_iterate(root, checkTopLevel);
    // maybe create global variables
    table_iterate(root, checkAllStatic);
    table_iterate(root, checkOperatorAssignmentType);

    //free global variables
    symTable = NULL;
}

//----- Check return type -----


void checkReturnPresenceC(Function *f, Command *c) {
    (void) f;
    if (c->type == C_RETURN) {
        hasReturnCommand = true;
    }
}
void checkReturnPresence(Node *node) {
    if (node->type != N_FUNCTION) {
        return;
    }
    Function *f = node->data.function;
    hasReturnCommand = false;

    traverseCommands(f, checkReturnPresenceC, NULL);
    if (f->returnType != T_VOID && !hasReturnCommand) {
        fprintf(stderr, "In function %s:\n", f->name);
        MERROR(ERR_SEM_TYPECHECK, "No 'return' in a non-void function.");
    }
}

void checkBinaryReference_(char *name, char *typeM){
    //filter
    char *look = strchr(name,'.');
    ++look;
    //end of filter
    Node *n=table_lookup_either(symTable, localTable, className, look);
    if (n == NULL)
        FERROR(ERR_SEM_UNDEFINED, "Trying to call undefined %s '%s'.", typeM, name);
}

void checkBinaryCond_(Expression *e){
    if (e->data.binary.left->data.reference != NULL){
        checkBinaryReference_(e->data.binary.left->data.reference, "reference");
    }
    else if (e->data.binary.right->data.reference != NULL)
        checkBinaryReference_(e->data.binary.right->data.reference, "reference");
    else if (e->data.binary.left->data.reference != NULL && e->data.binary.right->data.reference != NULL){
        checkBinaryReference_(e->data.binary.left->data.reference, "reference");
        checkBinaryReference_(e->data.binary.right->data.reference, "reference");
    }
    if (e->data.binary.left->data.funcall.name != NULL)
        checkBinaryReference_(e->data.binary.left->data.funcall.name , "function");
    else if (e->data.binary.right->data.funcall.name != NULL)
        checkBinaryReference_(e->data.binary.right->data.funcall.name, "function");
    else if (e->data.binary.left->data.funcall.name != NULL && e->data.binary.right->data.funcall.name != NULL){
        checkBinaryReference_(e->data.binary.left->data.funcall.name,"function");
        checkBinaryReference_(e->data.binary.right->data.funcall.name, "function");
    }
}

void checkCondition_(Command *c){
    Node *n; char *name;
    Expression *e;

    /* initialization of name */
    if (c->type == C_IF)
        e=c->data.ifC.cond;
    else if (c->type == C_WHILE)
        e=c->data.whileC.cond;
    else if (c->type == C_DO_WHILE)
        e=c->data.doWhileC.cond;
    else if (c->type == C_FOR)
        e=c->data.forC.cond;
    else
        return;

    switch(e->type){
    case E_FUNCALL:
        name = e->data.funcall.name;
        n=table_lookup_either(symTable, localTable, className, name);
        if (n == NULL || (c->type != C_DECLARE && c->type != C_DEFINE))
            FERROR(ERR_SEM_UNDEFINED, "Trying to call undefined function '%s'.", name);
        break;
    case E_REFERENCE:
        name = e->data.reference;
        n=table_lookup_either(symTable, localTable, className, name);
        if (n == NULL)
            FERROR(ERR_SEM_UNDEFINED, "Trying to call undefined reference '%s'.", name);
        break;
    case E_BINARY:
        checkBinaryCond_(e);
        break;
    case E_VALUE:
        break;
    }
}
void checkFnExpression(Function *f, Command *c){
    (void)f;
    //end
    if (c == NULL)
        return;
    char *look;
    Node *n;
    ValueType ltype, rtype;
    switch(c->type){
    case C_DECLARE:
        table_insert_dummy(localTable, c->data.declare);
        break;
    case C_DEFINE:
        ltype = c->data.define.declaration.type;
        rtype = getExpressionType(c->data.define.expr);
        if (!checkAssignCompatible(ltype, rtype)) {
            FERROR(ERR_SEM_TYPECHECK, "Cannot assign a(n) %s to %s.\n",
                   showValueType(rtype), showValueType(ltype));
        }
        table_insert_dummy(localTable, c->data.define.declaration);
        break;
    case C_ASSIGN:
        look = strchr(c->data.assign.name,'.');
        ++look;
        n = table_lookup_either(symTable, localTable, className, look);
        if (n == NULL) {
            fprintf(stderr, "In function %s:\n", f->name);
            FERROR(ERR_SEM_UNDEFINED, "Trying to assign to undefined variable '%s'", look);
        }
        break;
    case C_IF:
    case C_WHILE:
    case C_DO_WHILE:
    case C_EXPRESSION:
    case C_RETURN:
        checkCondition_(c);
        break;
    case C_FOR:
        table_insert_dummy(localTable, c->data.forC.var);
        checkCondition_(c);
        break;
    case C_CONTINUE:
    case C_BREAK:
    case C_BLOCK:
        break;
    }
}

void checkAllStatic(Node *node){
    if (node->type == N_FUNCTION){
        int i = 0;
        Function *f = node->data.function;
        while (f->name[i] != '.' && f->name[i] != '\0')
            i++;
        if (f->name[i] == '\0') {
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_INTERNAL, "Unqualified function name in symbol table");
        }
        className = malloc(sizeof(char) * (i + 1));
        strncpy(className, f->name, i);
        className[i] = '\0';

        localTable = createSymbolTable();
        Declaration *arg = f->argHead;
        while (arg != NULL) {
            table_insert_dummy(localTable, *arg);
        }

        traverseCommands(f, checkFnExpression, checkOperatorAssignmentTypeF);
        freeSymbolTable(localTable);
        localTable = NULL;
        free(className);
        className = NULL;
    }
}

void checkMainRun(SymbolTable *table){
    Node *tmp = table_lookup (table, "Main");
    if (tmp == NULL || tmp->type != N_CLASS){
        MERROR(ERR_SEM_UNDEFINED, "Missing class Main.");
    }
    tmp = table_lookup(table, "Main.run");
    if (tmp == NULL || tmp->type != N_FUNCTION)
        MERROR(ERR_SEM_UNDEFINED, "Missing run method in Main.");
}

ValueType coerceBinary(BinaryOperation op, ValueType left, ValueType right) {
    if (left == T_VOID || right == T_VOID) {
        return T_VOID; //this should never happen
    }
    if (left == T_STRING || right == T_STRING) {
        if (op == EB_ADD) {
            return T_STRING; //concatenation
        }
        FERROR(ERR_SEM_TYPECHECK, "Wrong operator types for operation %s: %s, %s.\n",
               showBinaryOperation(op), showValueType(left), showValueType(right));
    }
    if (left == T_BOOLEAN || right == T_BOOLEAN) {
        if (left == T_BOOLEAN && right == T_BOOLEAN &&
            (op == EB_EQUAL || op == EB_NOT_EQUAL)) {
            return T_BOOLEAN; //bool comparison
        }
        FERROR(ERR_SEM_TYPECHECK, "Wrong operator types for operation %s: %s, %s.\n",
               showBinaryOperation(op), showValueType(left), showValueType(right));
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

    FERROR(ERR_SEM_TYPECHECK, "Wrong operator types for operation %s: %s, %s.\n",
          showBinaryOperation(op), showValueType(left), showValueType(right));
}
bool checkAssignCompatible(ValueType lvalue, ValueType rvalue) {
    if ((lvalue == T_INTEGER || lvalue == T_DOUBLE) &&
        (rvalue == T_INTEGER || rvalue == T_DOUBLE)) {
        return true;
    }
    if (lvalue == T_STRING) {
        return true;
    }
    if (lvalue == T_BOOLEAN && rvalue == T_BOOLEAN) {
        return true;
    }
    return false;
}

ValueType getExpressionType(Expression *e) {
    if (e == NULL) {
        MERROR(ERR_INTERNAL, "NULL expression");
    }
    Node *n;
    switch (e->type) {
    case E_VALUE:
        return e->data.value->type;
    case E_FUNCALL:
        n = table_lookup(symTable, e->data.funcall.name);
        if (n->type != N_FUNCTION)
            MERROR(ERR_SEM_TYPECHECK, "Trying to call a variable");
        return n->data.function->returnType;
    case E_REFERENCE:
        n = table_lookup_either(symTable, localTable, className, e->data.reference);
        if (n == NULL)
            FERROR(ERR_SEM_TYPECHECK, "Trying to reference a nonesistent variable %s",
                   e->data.reference);
        if (n->type != N_VALUE)
            MERROR(ERR_SEM_TYPECHECK, "Trying to reference a function");
        return n->data.value->type;
    case E_BINARY:
        return coerceBinary(e->data.binary.op,
                            getExpressionType(e->data.binary.left),
                            getExpressionType(e->data.binary.right));
    }

    //...and appease the compiler
    return T_VOID;
}
void checkOperatorAssignmentTypeC(Function *f, Command *c) {
    Node *n;
    ValueType ltype, rtype;
    switch (c->type) {
    case C_DECLARE:
        table_insert_dummy(localTable, c->data.declare);
        break;
    case C_DEFINE:
        ltype = c->data.define.declaration.type;
        rtype = getExpressionType(c->data.define.expr);
        if (!checkAssignCompatible(ltype, rtype)) {
            FERROR(ERR_SEM_TYPECHECK, "Cannot assign a(n) %s to %s.\n",
                   showValueType(rtype), showValueType(ltype));
        }
        table_insert_dummy(localTable, c->data.define.declaration);
        break;
    case C_ASSIGN:
        n = table_lookup_either(symTable, localTable, className, c->data.assign.name);
        if (n->type != N_VALUE) {
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_SEM_TYPECHECK, "Can't assign to a function");
        }
        ltype = n->data.value->type;
        rtype = getExpressionType(c->data.assign.expr);
        if (!checkAssignCompatible(ltype, rtype)) {
            FERROR(ERR_SEM_TYPECHECK, "Cannot assign a(n) %s to %s.\n",
                   showValueType(rtype), showValueType(ltype));
        }
        break;
    case C_IF:
        if (!checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.ifC.cond))) {
            MERROR(ERR_SEM_TYPECHECK, "'if' condition requires a boolean expression\n");
        }
        break;
    case C_WHILE:
        if (!checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.whileC.cond))) {
            MERROR(ERR_SEM_TYPECHECK, "'while' condition requires a boolean expression\n");
        }
        break;
    case C_DO_WHILE:
        if (!checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.doWhileC.cond))) {
            MERROR(ERR_SEM_TYPECHECK, "'do-while' condition requires a boolean expression\n");
        }
        break;
    case C_FOR:
        ltype = c->data.forC.var.type;
        rtype = getExpressionType(c->data.doWhileC.cond);
        if (!checkAssignCompatible(ltype, rtype)) {
            FERROR(ERR_SEM_TYPECHECK, "Can't assign %s to %s in 'for' initialization",
                   showValueType(ltype), showValueType(rtype));
        }
        table_insert_dummy(localTable, c->data.forC.var);
        if (!checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.forC.cond))) {
            MERROR(ERR_SEM_TYPECHECK, "'for' condition requires a boolean expression\n");
        }
        break;
    case C_EXPRESSION:
        break;
    case C_RETURN:
        if (f->returnType == T_VOID) {
            if (c->data.expr != NULL) {
                fprintf(stderr, "In function %s:\n", f->name);
                MERROR(ERR_SEM_TYPECHECK, "Returning a value from a void function");
            }
        } else {
            if (c->data.expr == NULL) {
                fprintf(stderr, "In function %s:\n", f->name);
                MERROR(ERR_SEM_TYPECHECK, "Not returning a value from a non-void function");
            }
            ltype = f->returnType;
            rtype = getExpressionType(c->data.expr);
            if (!checkAssignCompatible(ltype, rtype)) {
                FERROR(ERR_SEM_TYPECHECK, "Can't return a %s from a function returning %s\n",
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
void checkOperatorAssignmentType(Node *node) {
    if (node->type != N_FUNCTION) {
        return;
    }
    Function *f = node->data.function;

    //find the class name
    int i = 0;
    while (f->name[i] != '.' && f->name[i] != '\0')
        i++;
    if (f->name[i] == '\0') {
        fprintf(stderr, "In function %s:\n", f->name);
        MERROR(ERR_INTERNAL, "Unqualified function name in symbol table");
    }
    className = malloc(sizeof(char) * (i + 1));
    strncpy(className, f->name, i);
    className[i] = '\0';

    localTable = createSymbolTable();
    Declaration *arg = f->argHead;
    printDeclaration(arg);
    while (arg != NULL) {
        table_insert_dummy(localTable, *arg);
        arg = arg->next;
    }
    traverseCommands(f, checkOperatorAssignmentTypeC,
                     checkOperatorAssignmentTypeF);
    freeSymbolTable(localTable);
    localTable = NULL;
    free(className);
    className = NULL;
}

void checkTopLevelInner(Function *f, Command *c) {
    while (c != NULL) {
        switch (c->type) {
        case C_DECLARE:
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_INTERNAL, "C_DECLARE located out of top-level of command");
            break;
        case C_DEFINE:
            fprintf(stderr, "In function %s:\n", f->name);
            MERROR(ERR_INTERNAL, "C_DEFINE located out of top-level of command");
            break;
        case C_ASSIGN:
            break;
        case C_IF:
            checkTopLevelInner(f, c->data.ifC.thenBlock.head);
            checkTopLevelInner(f, c->data.ifC.elseBlock.head);
            break;
        case C_WHILE:
            checkTopLevelInner(f, c->data.whileC.bodyBlock.head);
            break;
        case C_DO_WHILE:
            checkTopLevelInner(f, c->data.doWhileC.bodyBlock.head);
            break;
        case C_FOR:
            checkTopLevelInner(f, c->data.forC.iter);
            checkTopLevelInner(f, c->data.forC.bodyBlock.head);
            break;
        case C_EXPRESSION:
            break;
        case C_RETURN:
            break;
        case C_BLOCK:
            checkTopLevelInner(f, c->data.block.head);
            break;
        case C_CONTINUE:
        case C_BREAK:
            break;
        }
        c = c->next;
    }
}

// Aplikace v pruchodu stromu
void checkTopLevel(Node *node) {
    if (node->type != N_FUNCTION) {
        return;
    }
    Function *f = node->data.function;
    for (Command *c = f->body.head; c != NULL; c = c->next) {
        switch (c->type) {
        case C_IF:
            checkTopLevelInner(f, c->data.ifC.thenBlock.head);
            checkTopLevelInner(f, c->data.ifC.elseBlock.head);
            break;
        case C_WHILE:
            checkTopLevelInner(f, c->data.whileC.bodyBlock.head);
            break;
        case C_DO_WHILE:
            checkTopLevelInner(f, c->data.doWhileC.bodyBlock.head);
            break;
        case C_FOR:
            checkTopLevelInner(f, c->data.forC.iter);
            checkTopLevelInner(f, c->data.forC.bodyBlock.head);
            break;
        case C_BLOCK:
        case C_DECLARE:
        case C_DEFINE:
        case C_ASSIGN:
        case C_EXPRESSION:
        case C_RETURN:
        case C_CONTINUE:
        case C_BREAK:
            break;
        }
    }
}
