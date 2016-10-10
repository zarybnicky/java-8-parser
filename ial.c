/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

// TODO - List-Merge sort, tab. sym. binarnim vyhledavacim stromem, vyhledavani podretezce Knuth-Morris-Pratt
// Merge-sort - nutno upravit dle inplementace stringu

#include "ial.h"

char *substr(char *s, unsigned start, unsigned length)
{
    return "";
}

char *sort(char *s)
{
    unsigned len = strlen(s);

    if (len != 1) {  // Rozdeleni na mensi stringy
        unsigned x = 0;
        unsigned y = 0;
        unsigned count = 0; // Pocitadlo prvku pole
        unsigned half = len / 2;

        char *left = substr(s, 0, half);
        char *right;
        unsigned top;
        if (len % 2 == 0) { // char *sudy X lichy pocet clenu
            right = substr(s, half, half);
            top = half;
        } else {
            right = substr(s, half, half + 1);
            top = half+1;

        }

        left = sort(left);  // Rekurzivni volani fce.
        right = sort(right);

        while (x < half && y < top) {  // Sort -> Pocitam u obou retezcu od 0 - top muze byt ichy nebo sudy (viz. vyse)
            if (left[x] <= right[y]){
                s[count] = left[x];
                x++;
            } else {
                s[count] = right[y];
                y++;
            }   // if
            count++;
        }   // while

        while (x < half){   // Doplneni zbyvajicich znaku
            s[count] = left[x];
            count++;
        }


        }

        left = sort(left);  // Rekurzivni volani fce.
        right = sort(right);

        while (x < half && y < top) {  // Sort -> Pocitam u obou retezcu od 0 - top muze byt ichy nebo sudy (viz. vyse)
            if (left[x] <= right[y]){
                s[count] = left[x];
                x++;
            } else {
                s[count] = right[y];
                y++;
            }   // if
            count++;
        }   // while

        while (x < half){   // Doplneni zbyvajicich znaku
            s[count] = left[x];
            count++;
        }


        while (y < top){
            s[count] = right[y];
            count++;
        }

    }   // if (len != 1)
    return s;
}   // function


sym_table *create_symbol_table()
{

}

void insert_symbol(sym_table *s_table, symbol *sym, symbol_obj *arr)
{

}

symbol_obj lookup_symbol(sym_table *s_table, symbol *sym)
{

}

