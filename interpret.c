/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

// NOTE compile with -lpthread

#include "interpret.h"



int int_initialize(void){
    return sem_init(&sem_interpret, 0,1);
}

int int_looper(int *instruction){
    while(*instruction != HALT){
        ;
    }
    printf("%s\n", "O Hi Mark!");
    return 0;
}

Interpret *createInterpret() {
    Interpret *i = malloc(sizeof(Interpret));
    CHECK_ALLOC(i);
    i->symTable.root = NULL;
    return i;
}
void freeInterpret(Interpret *i) {
    if (i == NULL)
        return;
    if (i->symTable.root != NULL)
        freeNode(i->symTable.root);
    free(i);
}

void evalMain(Interpret *i) {
    Node *mainFn = table_lookup(&i->symTable, "Main.run");
    if (mainFn == NULL || mainFn->type != N_FUNCTION) {
        MERROR(ERR_SEM_UNDEFINED, "Required method Main.run is not defined");
    }
    printf("Not implemented\n");
}

Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right) {
    switch (op) {
    case EB_EQUAL:
    case EB_NOT_EQUAL:
    case EB_LESS:
    case EB_LESS_EQUAL:
    case EB_GREATER:
    case EB_GREATER_EQUAL:
    case EB_MULTIPLY:
    case EB_DIVIDE:
    case EB_ADD:
    case EB_SUBTRACT:
        (void) right;
        return left; //FIXME!!!
    }
    return NULL; //Just to pacify the compiler...
}

Value *evalStaticExpression(Expression *e) {
    Value *left, *right;

    switch (e->type) {
    case E_FUNCALL:
    case E_REFERENCE:
        MERROR(ERR_SYNTAX, "Cannot use dynamic expressions during static variable initialization!");
    case E_VALUE:
        return e->data.value;
    case E_BINARY:
        left = evalStaticExpression(e->data.binary.left);
        right = evalStaticExpression(e->data.binary.right);
        return evalBinaryExpression(e->data.binary.op, left, right);
    }
    return NULL; //Just to pacify the compiler...
}
