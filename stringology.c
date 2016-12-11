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

int readInt() {
    int c = EOF - 1;
    unsigned vysledek = 0;

    while ((c = getchar()) != '\n' && c != EOF) {
        if (c < '0' || '9' < c) {
            FERROR(ERR_RUNTIME_INT_PARSE, "Unexpected character: %c\n", c);
        }
        vysledek = vysledek * 10 + (c - '0');
    }
    if (c == EOF - 1) {
        MERROR(ERR_RUNTIME_INT_PARSE, "No input\n");
    }
    return vysledek;
}

double readDouble() {
    Token *t = getNextToken(stdin);

    if (t->lineNum != -1){
        freeToken(t);
        MERROR(ERR_RUNTIME_INT_PARSE, "Unexpected character\n");
    }
    switch(t->type) {
        case LIT_DOUBLE:;
            double tmp = t->val.doubleVal;
            freeToken(t);
            return tmp;
        case LIT_INTEGER:;
            double pom = (double) t->val.intVal;
            freeToken(t);
            return pom;
        default:
            freeToken(t);
            MERROR(ERR_RUNTIME_INT_PARSE, "Unexpected character\n");
    }
}

char *readString() {
    unsigned len = 10;
    int c;
    unsigned i = 0;

    char *s = malloc_c(len * sizeof(char));

    while ((c = getchar()) != '\n' && c != EOF) {
        if (i + 1 == len) {
            len *= 2;
            s = realloc_c(s, len);
        }
        s[i++] = c;
    }
    s[i] = '\0';
    return s;
}

char *substr(char *s,int i, int n) {
    if (s == NULL || *s == '\0' || i < 0 || n < 0)
        ERROR(ERR_RUNTIME_MISC);
    int sLen = strlen(s);

    if (sLen - 1 < i || sLen - 1 < n - 1 + i)
        ERROR(ERR_RUNTIME_MISC);

    if (n != 0) {
        sLen = n+1;	// Set length
    } else {
        n = sLen - i;
        sLen = n+1;
    }

    char *pom = malloc_c(sLen);

    strncpy(pom, s+i, n);

    pom[sLen-1] = '\0';
    return pom; // Deallocation connected to ht_table
}
