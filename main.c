#include "scanner.h"


int main(){
FILE *F=fopen("test/2_no_class.java","r");
Token *t;
t=getNextToken(F);
return 0; }
