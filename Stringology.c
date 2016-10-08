/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "stringology.h"
#include <string.h>
 
int readInt();
double readDouble();
String readString();
void print();

int length(String s){
    int x = strlen(s);
    return x;
}

String substr(String s,int i, int n);
int compare(String s1, String s2);


 
