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
    //table_iterate(root, checkMainRun);
    //table_iterate(root, checkTopLevel);
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
