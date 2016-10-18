/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "ir.h"
#include "parser.h"

Lexer *l;
Interpret *i;
void freeGlobalResources() {
    freeLexer(l);
    freeInterpret(i);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        FERROR(ERR_INTERNAL,
               "Invalid arguments, usage: %s <FILENAME>\n", argv[0]);
    }
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        PERROR("Error while opening file");
    }

    i = createInterpret();

    l = createLexer(f, i);
    while (hasMore(l)) {
        parseClass(l);
    }
    fclose(f);

    evalMain(i);
    return 0;
}
