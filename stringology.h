/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_STRINGOLOGY_H
#define IFJ_STRINGOLOGY_H

#include "int_memory_management.h"
#include <ctype.h>
#include "scanner.h"

int readInt();
double readDouble();
char *readString();
char *substr(char *s, int where, int howmany);

#endif /* IFJ_STRINGOLOGY_H */
