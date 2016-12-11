/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "../interpret.h"

int main() {
    printf("%s\n", "TEST STACK");

    printf("%s\n", "1. create stack");

    Stack *stack = NULL;

    stack = createLocalStack(NULL);

    printStack(stack);

    printf("%s\n", "\n--------------------\n");

    printf("%s\n", "2. push to stack");


    Value *val = createValue(T_INTEGER);
    val->data.integer = 42;
    pushToStack(stack, val);

    val = createValue(T_DOUBLE);
    val->data.dbl = 41.0;
    pushToStack(stack, val);

    val = createValue(T_STRING);

    val->data.str = malloc(sizeof(strlen("asdf")) + 1);
    strcpy(val->data.str, "asdf\0");

    pushToStack(stack, val);

    printStack(stack);
    printf("%s\n", "--------------------\n");

    printf("%s\n", "3. pop to stack");

    popFromStack(stack);
    printStack(stack);
    printf("\n\n");
    popFromStack(stack);
    printStack(stack);
    printf("\n\n");

    popFromStack(stack);
    printStack(stack);
    printf("\n\n");

    popFromStack(stack);
    printStack(stack);
    printf("\n\n");

    printf("%s\n", "--------------------\n");

    printf("%s\n", "END TEST\n This test do not free all resources it should left 4 memory blocks!");

    free_c_all(&alloc_tab);

    return 0;
}
