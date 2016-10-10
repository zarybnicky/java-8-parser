/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "interpreter.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Invalid arguments, usage: %s <FILENAME>\n", argv[0]);
        exit(ERR_INTERNAL);
    }
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("Error while opening file");
        exit(ERR_INTERNAL);
    }

    printf("Not implemented.");

    fclose(f);
    return 0;
}
