#include "scanner.h"

void freeGlobalResources() { }

int main(){
    FILE *F = fopen("test/2_no_class.java","r");
    Token *t = getNextToken(F);
    return 0;
}
