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
// TODO - Zacal jsem sort -> dodelam ho. Bohuzel, jsem musel jit. Jirka

#include "ial.h"

char *substr(char *s, unsigned start, unsigned length)
{
  return "";
}

char *sort(char *s)
{
    unsigned len = strlen(s);
    if (len != 1) {  // Rozdeleni Na mensi stringy
        unsigned half = len / 2;

        char *left = substr(s, 0, half);
        if (len % 2 == 0) { // char *sudy X lichy pocet clenu
            char *right = substr(s, half, half);
        } else {
            char *right = substr(s, half, half + 1);
        }
    }
}
