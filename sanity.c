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

    table_iterate(root, checkReturnType);
    checkMainRun(&i->symTable);
    // maybe create global variables
    //table_iterate(root, checkAllStatic);
    table_iterate(root, checkOperatorAssignmentType);

    //free global variables
    symTable = NULL;
}

//----- Check return type -----


void checkReturnCommand(Function *f, Command *c) {
    if (c->type != C_RETURN) {
        return;
    }
    hasReturnCommand = true;

    if (f->returnType == T_VOID) {
        if (c->data.expr != NULL)
            MERROR(ERR_SEM_TYPECHECK, "Trying to return a value from a void function");
        return;
    }
    Node *n; char *name;
    switch (c->data.expr->type) {
    case E_VALUE:
        if (f->returnType != c->data.expr->data.value->type)
            //FIXME: implicit conversions
            MERROR(ERR_SEM_TYPECHECK, "Trying to return a value with an incompatible type.");
        break;
    case E_FUNCALL:
        name = c->data.expr->data.funcall.name;
        n = table_lookup(symTable, name);
        if (n == NULL)
            FERROR(ERR_SEM_UNDEFINED, "Trying to call an undefined function '%s'.", name);
        if (n->type == N_VALUE)
            FERROR(ERR_SEM_TYPECHECK, "Trying to call a variable '%s'.", name);
        if (n->data.function->returnType != f->returnType)
            //FIXME: implicit conversions
            MERROR(ERR_SEM_TYPECHECK, "Returning a function call with an incompatible type.");
        break;
    case E_REFERENCE:
        name = c->data.expr->data.funcall.name;
        n = table_lookup(symTable, name);
        if (n == NULL)
            FERROR(ERR_SEM_UNDEFINED, "Trying to return an undefined variable %s", name);
        if (n->type == N_FUNCTION)
            MERROR(ERR_SEM_TYPECHECK, "Trying to return a function.");
        if (n->data.value->type != f->returnType)
            //FIXME: implicit conversions
            MERROR(ERR_SEM_TYPECHECK, "Trying to return a variable with an incompatible type.");
        break;
    case E_BINARY:
        //FIXME
        MERROR(ERR_INTERNAL, "Not yet implemented.");
        break;
    }
}
void checkReturnType(Node *node) {
    if (node->type != N_FUNCTION) {
        return;
    }
    Function *f = node->data.function;
    hasReturnCommand = false;
    printFunction(f);
    traverseCommands(f, checkReturnCommand, NULL);
    if (f->returnType != T_VOID && !hasReturnCommand) {
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
        FERROR(ERR_SEM_UNDEFINED, "Trying to call undefined %s '%s'.",typeM,name);
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

    switch(e->type){
    case E_FUNCALL:
        name = e->data.funcall.name;
        n=table_lookup_either(symTable, localTable, className, name);
        if (n == NULL || (c->type != C_DECLARE && c->type != C_DEFINE) )
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
    switch(c->type){
    case C_DECLARE:
        table_insert_dummy(localTable, c->data.declare);
        break;
    case C_DEFINE:
        checkAssignCompatible(c->data.define.declaration.type,
        getExpressionType(c->data.define.expr));
        table_insert_dummy(localTable, c->data.define.declaration);
        break;
    case C_ASSIGN:
        look = strchr(c->data.assign.name,'.');
        ++look;
        n = table_lookup_either(symTable, localTable, className, look);
        if (n == NULL)
            FERROR(ERR_SEM_UNDEFINED, "Trying to assign to undefined variable '%s'", look);
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

void checkAllStatic (Node *node){
    if (node->type == N_FUNCTION){
        int i = 0;
        Function *f = node->data.function;
        while (f->name[i] != '.' && f->name[i] != '\0')
            i++;
        if (f->name[i] == '\0')
            MERROR(ERR_INTERNAL, "Unqualified function name in symbol table");
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
        fprintf(stderr, "Wrong operator types for operation %s: %s, %s.\n",
                showBinaryOperation(op),
                showValueType(left), showValueType(right));
        ERROR(ERR_SEM_TYPECHECK);
    }
    if (left == T_BOOLEAN || right == T_BOOLEAN) {
        if (left == T_BOOLEAN && right == T_BOOLEAN &&
            (op == EB_EQUAL || op == EB_NOT_EQUAL)) {
            return T_BOOLEAN; //bool comparison
        }
        fprintf(stderr, "Wrong operator types for operation %s: %s, %s.\n",
                showBinaryOperation(op),
                showValueType(left), showValueType(right));
        ERROR(ERR_SEM_TYPECHECK);
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

    //...and to appease the compiler
    fprintf(stderr, "Wrong operator types for operation %s: %s, %s.\n",
            showBinaryOperation(op),
            showValueType(left), showValueType(right));
    ERROR(ERR_SEM_TYPECHECK);
}
void checkAssignCompatible(ValueType lvalue, ValueType rvalue) {
    if ((lvalue == T_INTEGER || lvalue == T_DOUBLE) &&
        (rvalue == T_INTEGER || rvalue == T_DOUBLE)) {
        return;
    }
    if (lvalue == T_STRING) {
        return;
    }
    if (lvalue == T_BOOLEAN && rvalue == T_BOOLEAN) {
        return;
    }
    fprintf(stderr, "Cannot assign a %s to %s.\n",
            showValueType(rvalue), showValueType(lvalue));
    ERROR(ERR_SEM_TYPECHECK);
}

ValueType getExpressionType(Expression *e) {
    if (e == NULL) {
        MERROR(ERR_INTERNAL, "NULL expression");
    }
    Node *n;
    ValueType l, r;
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
        if (n->type != N_VALUE)
            MERROR(ERR_SEM_TYPECHECK, "Trying to reference a function");
        return n->data.value->type;
    case E_BINARY:
        l = getExpressionType(e->data.binary.left);
        r = getExpressionType(e->data.binary.right);
        return coerceBinary(e->data.binary.op, l, r);
    }

    //...and appease the compiler
    return T_VOID;
}
void checkOperatorAssignmentTypeC(Function *f, Command *c) {
    Node *n;
    switch (c->type) {
    case C_DECLARE:
        table_insert_dummy(localTable, c->data.declare);
        break;
    case C_DEFINE:
        checkAssignCompatible(c->data.define.declaration.type,
        getExpressionType(c->data.define.expr));
        table_insert_dummy(localTable, c->data.define.declaration);
        break;
    case C_ASSIGN:
        n = table_lookup_either(symTable, localTable, className, c->data.assign.name);
        if (n->type != N_VALUE)
            MERROR(ERR_SEM_TYPECHECK, "Can't assign to a function");
        checkAssignCompatible(n->data.value->type,
        getExpressionType(c->data.assign.expr));
        break;
    case C_IF:
        checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.ifC.cond));
        break;
    case C_WHILE:
        checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.whileC.cond));
        break;
    case C_DO_WHILE:
        checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.doWhileC.cond));
        break;
    case C_FOR:
        checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.forC.initial));
        table_insert_dummy(localTable, c->data.forC.var);
        checkAssignCompatible(T_BOOLEAN, getExpressionType(c->data.forC.cond));
        break;
    case C_EXPRESSION:
        checkAssignCompatible(T_STRING, getExpressionType(c->data.expr)); //universal type
        break;
    case C_RETURN:
        if (f->returnType == T_VOID) {
            if (c->data.expr != NULL) {
                MERROR(ERR_SEM_TYPECHECK,
                       "Returning a value from a void function");
            }
        } else {
            if (c->data.expr == NULL) {
                MERROR(ERR_SEM_TYPECHECK,
                       "Not returning a value from a non-void function");
            }
            checkAssignCompatible(f->returnType, getExpressionType(c->data.expr));
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
    if (f->name[i] == '\0')
        MERROR(ERR_INTERNAL, "Unqualified function name in symbol table");
    className = malloc(sizeof(char) * (i + 1));
    strncpy(className, f->name, i);
    className[i] = '\0';

    localTable = createSymbolTable();
    Declaration *arg = f->argHead;
    while (arg != NULL) {
        table_insert_dummy(localTable, *arg);
    }
    traverseCommands(f, checkOperatorAssignmentTypeC,
                     checkOperatorAssignmentTypeF);
    freeSymbolTable(localTable);
    localTable = NULL;
    free(className);
    className = NULL;
}
