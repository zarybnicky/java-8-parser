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

int numberOfNum(char *s){
    int num = 0;
    while (*s != '\0'){
        if (isdigit(*s))
            num++;
        s++;
    }
    return num;
}

int readInt() { // Int celé nezáporné číslo (3.1)
    int c; // Urceno dle max delky int
    unsigned vysledek = 0;
    bool empty=true;
    while ((c = getchar()) != '\n' && c != EOF) {
        empty=false;
        if (c < '0' || '9' < c) {
            fprintf(stderr, "Error while parsing an integer, unexpected character: %c\n", c);
            return ERR_RUNTIME_INT_PARSE;
        }
        vysledek = vysledek*10 + (c - '0'); // Prepis na cislo
    }
    if ( empty == true ){
        //return ERR_RUNTIME_INT_PARSE
        fprintf(stderr, "Error while parsing an integer, unexpected character: %c\n", c);
        return ERR_RUNTIME_INT_PARSE;
    }
    return vysledek;
}

double readDouble() {
    Token *t = getNextToken(stdin);

    if (t->lineNum != -1){
        freeToken(t);
        fprintf(stderr, "Error while parsing an double, unexpected character\n");
        return ERR_RUNTIME_INT_PARSE;
    }
    switch(t->type){
        case LIT_DOUBLE:;
            double tmp = t->val.doubleVal;
            freeToken(t);
            return tmp;
        case LIT_INTEGER:;
            double pom = (double)t->val.intVal;
            freeToken(t);
            return pom;

        default:
            freeToken(t);
            fprintf(stderr, "Error while parsing an double, unexpected character\n");
            return ERR_RUNTIME_INT_PARSE;
    }
}

char *readString() {
    unsigned int Len = MAX_LEN;
    int c;
    unsigned int i = 0;

    char *Str = calloc_c(Len,sizeof(char));

    *Str = '\0';

    while ((c = getchar()) != '\n' && c != EOF) {
        if (i == Len) {
            Len = Len + MAX_LEN;
            Str = realloc_c(Str, Len);
        }

        Str[i] = c;
        i++;
    }

    return Str; // Potreba casem uvolnit
}

int length(char *s) {
    return strlen(s);
}

char *substr(char *s,int i, int n) {
    if (s == NULL || *s == '\0' || i < 0 || n < 0)
        ERROR(ERR_RUNTIME_MISC);
    int sLen = strlen(s);

    if (sLen - 1 < i || sLen - 1 < n - 1 + i)
        ERROR(ERR_RUNTIME_MISC); //

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

int compare(char *s1, char *s2) {
    return strcmp(s1,s2);
}

void print(Value *term){

    switch(term->type){

        case T_STRING:
            printf("%s",term->data.str);
            break;
        case T_BOOLEAN:
            printf("%s",term->data.boolean ? "true" : "false");
            break;
        case T_DOUBLE:
            printf("%g",term->data.dbl);
            break;
        case T_INTEGER:
            printf("%d",term->data.integer);
            break;
        default:
            break;
    }
}
