#include "scanner.h"

void freeGlobalResources() { }

int main(){
    FILE *F = fopen("test/2_no_class.java","r");
    while (getNextToken(F) != NULL)
        ;
    fclose(F);
    return 0;
}
