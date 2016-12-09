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

    while ((c = getchar()) != '\n' && c != EOF) {
        if (c < '0' || '9' < c) {
            //return ERR_RUNTIME_INT_PARSE
            fprintf(stderr, "Error while parsing an integer, unexpected character: %c\n", c);
            return ERR_RUNTIME_INT_PARSE;
        }
        vysledek = vysledek*10 + (c - '0'); // Prepis na cislo
    }
    return vysledek;
}

double readDouble() { 
    //FILE *f = stdin;
    Token *t = getNextToken(stdin);
    double tmp = t->val.doubleVal;
    freeToken(t);
    return tmp;
}
/*
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
            //return ERR_RUNTIME_INT_PARSE
            fprintf(stderr, "Error while parsing a float, unexpected character: %c\n", c);
            free(Str);
            return ERR_RUNTIME_INT_PARSE;
        }

        switch(c) {
        case 'e':
        case 'E':
            if (e_set != 0) { // Vice exp za sebou, nebo exp neni cele cislo
                //return ERR_RUNTIME_INT_PARSE
                fprintf(stderr, "Error while parsing a float, unexpected character: %c\n", c);
                free(Str);
                return ERR_RUNTIME_INT_PARSE;
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
                fprintf(stderr, "Error while parsing a float, unexpected character: %c\n", c);
                free(Str);
                return ERR_RUNTIME_INT_PARSE;
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
    if ( numberOfNum(Str) == 0 ){
        fprintf(stderr, "Error while parsing a float, no number read\n");
        free(Str);
        return ERR_RUNTIME_INT_PARSE;
    }
    free(Str);
    return vysledek;
} // function end
*/

char *readString() {
    unsigned int Len = MAX_LEN;
    int c;
    unsigned int i = 0;

    char *Str = malloc_c(Len);

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
    while (*s1 != '\0' && *s2 != '\0'){
        s1++;
        s2++;
    }
    if (*s1 == '\0' && *s2 == '\0')
        return 0;
    else if (*s1 != '\0' && *s2 == '\0')
        return 1;
    else
        return -1;
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
