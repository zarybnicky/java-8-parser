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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "error.h"
#include "ir.h"
#include "int_memory_management.h"

int readInt();
double readDouble();
char *readString();
void print(Value *term);
int length(char *s);
char *substr(char *s, int i, int n);
int compare(char *s1, char *s2);

#endif /* IFJ_STRINGOLOGY_H */
