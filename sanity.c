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

void runSemanticAnalysis(Interpret *i) {
    symTable = &i->symTable;
    Node *root = symTable->root;

    table_iterate(root, checkReturnType);
    checkMainRun(&i->symTable);
    table_iterate(root, checkAllStatic);
}

//----- Check return type -----
static bool hasReturnCommand;
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
    traverseCommands(f, checkReturnCommand);
    if (f->returnType != T_VOID && !hasReturnCommand) {
        MERROR(ERR_SEM_TYPECHECK, "No 'return' in a non-void function.");
    }
}

void checkFnExpresion(Function *f, Command *c){
    (void)f;
    //FIXME ?!?!

    if (c->type == C_EXPRESSION){
        //printf("\n\n");
        //printCommand(c);
        //printf("%s\n\n\n", showExpressionType(c->data.expr->type));
        Node *n; char *name;
        switch(c->data.expr->type){
            case E_FUNCALL:
                name = c->data.expr->data.funcall.name;
                //FIXME GOBAL TABLE??!?
                n=table_lookup(symTable, name);
                if (n == NULL)
                    FERROR(ERR_SEM_UNDEFINED, "Trying to call undefined function '%s'.", name);
                break;
            case E_REFERENCE:
                name = c->data.expr->data.funcall.name;
                n=table_lookup(symTable, name);
                if ( n == NULL || (c->type != C_DECLARE ||
                    c->type != C_EXPRESSION) ){
                    FERROR(ERR_SEM_UNDEFINED,"Function '%s' not defined.", name);
                }
            default:
                break;
        }
    }
}

void checkAllStatic (Node *node){
    if (node->type == N_VALUE){
        Value *v = node->data.value;
        if (v->undefined == false)
            FERROR(ERR_SEM_UNDEFINED, "Value '%s' not defined.", node->symbol);
    }
    if (node->type == N_FUNCTION){
        Function *f = node->data.function;
        traverseCommands(f, checkFnExpresion);
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
