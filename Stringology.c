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

#define MAX_LEN 100

#include "stringology.h"
#include <string.h>
#include <stdio.h>
#include <stdlib>
#include <limits.h>
 
int readInt(){ // Int celé nezáporné číslo (3.1)
    char c; // Urceno dle max delky int

    unsigned int vysledek = 0;

    while ((c = getchar()) != '\n' && c != EOF){
        if ('0' <= c && c <= '9') vysledek = vysledek*10 + (c - '0'); // Prepis na cislo
        else exit(7); // Chyba 7
    }

    return vysledek;
}

double readDouble(){ // TODO
    unsigned int Len = MAX_LEN, i = 0;
    double vysledek = 0;
    char c;
    short e_set = 0, dot_set = 0;

    String Str = malloc(Len);
    if (Str == NULL) exit(99);

    while ((c = getchar()) != '\n' && c != EOF){
        if (i == Len){
            Len = Len + MAX_LEN;
            Str = realloc(Str, Len); // Zvetsi string
            if (Str == NULL) exit(99);

        }

        if (('0' <= c && c <= '9') || c != '.' || c != 'e' || c != 'E' || c != '+' || c != '-') exit(7); // Chyba 7

        switch(c){
            case 'e':
            case 'E': 
                if (e_set != 0) exit(7); // Chyba 7 - Vice exp za sebou, nebo exp neni cele cislo
                if (dot_set == 0){
                    for (unsigned int j = 0; j < i; j++){
                        vysledek = vysledek*10 + (Str[j] - '0');
                    }
                } else {
                    double pom = 0;
                    for (unsigned int j = 0; j < i; j++){
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
                if (e_set != 0 || dot_set != 0) exit(7); // Chyba 7
                for (unsigned int j = 0; j < i; j++){
                    vysledek = vysledek*10 + (Str[j] - '0');
                }
                i = 0;
                dot_set = 1;
                break;

            default:       
                Str[i] = c;
                i++;
                break;
        }

    }

    if (e_set == 0 || dot_set == 0){ // Jenom celé cislo
        for (unsigned int j = 0; j < i; j++){
            vysledek = vysledek*10 + (Str[j] - '0');
        }
    }
    if (e_set == 1){ // zpracovani exponentu TODO
        for (unsigned int j = 0; j < i; j++){
            vysledek = vysledek*10 + (Str[j] - '0');
        }
    }
    if (dot_set == 1){ //
        double pom = 0;
        for (unsigned int j = 0; j < i; j++){
            pom = pom*0.1 + (Str[j] - '0');
        }
        vysledek = vysledek + pom;
    }

    free(Str);
    return vysledek;
}

String readString(){
    unsigned int Len = MAX_LEN;
    char c;    
    unsigned int i = 0;

    String Str = malloc(Len);
    if (Str == NULL) exit(99);


    while ((c = getchar()) != '\n' && c != EOF){
        if (i == Len){
            Len = Len + MAX_LEN;
            Str = realloc(Str, Len);
            if (Str == NULL) exit(99);

        }

        Str[i] = c;
        i++;
    }

    return Str; // Potreba casem uvolnit

}

void print(){



}

int length(String s){
    return strlen(s);
}

String substr(String s,int i, int n){
    int sLen = strlen(s);

    if (sLen-1 < i) exit(10); // Dopsat chybu 10
    if (sLen-1 < i + n) exit(10); // Chceme prilis dlouhy podretezec -> udelame castecny?

    if (n != 0) sLen = n+1;	// Urceni delky
    else sLen = sLen-i;

    String pom = malloc(sLen); 
    if (pom == NULL) exit(99);

    strncpy(pom, s+i, n);

    pom[sLen-1] = '\0';
    return pom; // Nekdy dealokovat

}

int compare(String s1, String s2){
    unsigned int s1len = strlen(s1);
    unsigned int s2len = strlen(s2);

    if (s1len > s2len) return 1;
	if (s1len < s2len) return -1;

    for (unsigned int i = 0; i < s1len; i++){
        if (s1[i] > s2[i]) return 1;
		if (s1[i] < s2[i]) return -1;

    }

    return 0;

}
