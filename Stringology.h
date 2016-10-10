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
#include <stdlib>
#include <limits.h>

int readInt();
double readDouble();
String readString();
void print();
int length(String s);
String substr(String s,int i, int n);
int compare(String s1, String s2);

#endif /* IFJ_STRINGOLOGY_H */
