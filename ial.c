/*
* Projekt IFJ: IFJ16 
* FIT VUTBR
* Vytvořili:
*       xzaryb00 - Zarybnický Jakub
*       xtamas01 - Tamaškovič Marek
*       xvasko12 - Vaško Martin
*       xvasko14 - Vaško Michal
*       xzales12 - Záleský Jiří
* Datum: 1.10.2016
*/


// TODO - List-Merge sort, tab. sym. binarnim vyhledavacim stromem, vyhledavani podretezce Knuth-Morris-Pratt
// TODO - Zacal jsem sort -> dodelam ho. Bohuzel, jsem musel jit. Jirka

#include <stdlib.h>



String sort(String s){
    usigned int len = lenght(s);    //LENGTH - TODO - fce. na delku stringu 
    if (len != 1){  // Rozdeleni Na mensi stringy
        unsigned int half = len/2;
        
        string left = substr(s,0,half);    
        if (len%2 == 0){ // String sudy X lichy pocet clenu 
            string right = substr(s,half,half);        
        }else{
            string right = substr(s,half,half+1);    
        }
    }

}

int find(String s, String search);
