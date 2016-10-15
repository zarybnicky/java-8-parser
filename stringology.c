/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

// Dopsat chybu 10

#include "stringology.h"

#define MAX_LEN 10

int readInt() { // Int celé nezáporné číslo (3.1)
    int c; // Urceno dle max delky int
    unsigned vysledek = 0;

    while ((c = getchar()) != '\n' && c != EOF) {
        if (c < '0' && '9' < c) {
            FERROR(ERR_RUNTIME_INT_PARSE, "Error while parsing an integer, unexpected character: %c\n", c);
        }
        vysledek = vysledek*10 + (c - '0'); // Prepis na cislo
    }
    return vysledek;
}

double readDouble() { // TODO
    unsigned int Len = MAX_LEN, i = 0;
    double vysledek = 0;
    int c;
    short e_set = 0, dot_set = 0;

    char *Str = malloc(Len);
    CHECK_ALLOC(Str);

    while ((c = getchar()) != '\n' && c != EOF) {
        if (i == Len) {
            Len += MAX_LEN;
            Str = realloc(Str, Len); // Zvetsi string
            CHECK_ALLOC(Str);
        }

        if ((c < '0' || '9' < c) && c != '.' && c != 'e' && c != 'E' && c != '+' && c != '-') {
            FERROR(ERR_RUNTIME_INT_PARSE, "Error while parsing a float, unexpected character: %c\n", c);
        }

        switch(c) {
        case 'e':
        case 'E':
            if (e_set != 0) { // Vice exp za sebou, nebo exp neni cele cislo
                FERROR(ERR_RUNTIME_INT_PARSE, "Error while parsing a float, unexpected character: %c\n", c);
            }
            if (dot_set == 0) {
                for (unsigned j = 0; j < i; j++) { // Zapsani celych cisel
                    vysledek = vysledek*10 + (Str[j] - '0');
                }
            } else {
                double pom = 0;
                for (int j = strlen(Str)-1; j > -1; j--) { // Zapsani desetinnych mist
                    pom = pom*0.1 + (Str[j] - '0');
                }
                pom = pom*0.1; // Posledni posun
                vysledek = vysledek + pom;
                dot_set = 0;
            }
            i = 0;
            e_set = 1;
            break;

        case '.':
            if (e_set != 0 || dot_set != 0) { // Kontrola pro vice tecek nebo tecky po exp
                FERROR(ERR_RUNTIME_INT_PARSE, "Error while parsing a float, unexpected character: %c\n", c);
            }
            for (unsigned j = 0; j < i; j++) { // Zapsani celych cisel
                vysledek = vysledek*10 + (Str[j] - '0');
            }
            i = 0;
            dot_set = 1;
            break;

        default:
            Str[i++] = c; // Ulozeni znaku
            Str[i] = '\0'; // Nulovani dalsiho znaku
            break;
        } // Switch end
    }

    if (e_set == 0 && dot_set == 0) { // Jenom celé cislo
        for (unsigned j = 0; j < i; j++) {
            vysledek = vysledek*10 + (Str[j] - '0');
        }
    }
    if (e_set == 1) { // Zpracovani exponentu
        unsigned int pom = 0;

        if (Str[0] != '-'){
            // Kladne znamenku u exponentu
            for (unsigned int j = 0; j < i; j++) { // Prepis na cisla
                pom = pom*10 + (Str[j] - '0');
            }
            for (unsigned int j = 0; j < pom; j++) { // Aplikace exponentu
                vysledek = vysledek*10;
            }
        } else {
            // Zaporne znamenko u exponentu
            for (unsigned int j = 1; j < i; j++) { // Prepis na cisla
                pom = pom*10 + (Str[j] - '0');
            }
            for (unsigned int j = 0; j < pom; j++) { // Aplikace exponentu
                vysledek = vysledek*0.1;
            }
        } // End if Str[0] != '-'
    }

    if (dot_set == 1) { // Doplneni desetinnych mist
        double pom = 0;

        for (int j = strlen(Str)-1; j > -1; j--) { // Zapsani desetinnych mist
            pom = pom*0.1 + (Str[j] - '0');
        }

        pom = pom*0.1; // Posledni posun
        vysledek = vysledek + pom;
    }

    free(Str);
    return vysledek;
} // function end

char *readString() {
    unsigned int Len = MAX_LEN;
    int c;
    unsigned int i = 0;

    char *Str = malloc(Len);
    CHECK_ALLOC(Str);

    while ((c = getchar()) != '\n' && c != EOF) {
        if (i == Len) {
            Len = Len + MAX_LEN;
            Str = realloc(Str, Len);
            CHECK_ALLOC(Str);
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
    int sLen = strlen(s);

    if (sLen - 1 < i || sLen - 1 < i + n)
        ERROR(ERR_RUNTIME_MISC); // Chceme prilis dlouhy podretezec -> udelame castecny?

    if (n != 0) {
        sLen = n+1;	// Urceni delky
    } else {
        n = sLen - i;
        sLen = n+1;
    }

    char *pom = malloc(sLen);
    CHECK_ALLOC(pom);

    strncpy(pom, s+i, n);

    pom[sLen-1] = '\0';
    return pom; // Nekdy dealokovat
}

int compare(char *s1, char *s2) {
    unsigned int s1len = strlen(s1);
    unsigned int s2len = strlen(s2);

    if (s1len > s2len)
        return 1;
    if (s1len < s2len)
        return -1;

    for (unsigned int i = 0; i < s1len; i++) {
        if (s1[i] > s2[i])
            return 1;
        if (s1[i] < s2[i])
            return -1;
    }
    return 0;
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
