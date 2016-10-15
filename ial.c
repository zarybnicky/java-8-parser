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

int *Prefixcreator(char *search, int seaLen){ // Pomocna funkce pro find
    int *array = malloc(sizeof(int)*seaLen); // Prefixove pole
    CHECK_ALLOC(array);

    int a = -1;
    array[0] = a;

    for (int i = 1; i < seaLen; i++){ // Pruchod stringem
        while (a > -1 && search[a+1] != search[i]){ // Neni shoda
            a = array[a];
        }
        if (search[a+1] == search[i]){ // Zvyseni pri shodnosti
            a++;
        }
        array[i] = a; // Zapis hodnoty shodnych
    }
    return array;
} // Funkce Prefixcreator

int find(char *s, char *search) {
    int sLen = strlen(s);
    int seaLen = strlen(search);
    if (seaLen == 0) {
        return 0;
    }

    int *Parray = Prefixcreator(search, seaLen); // Volani pomocne funkce (viz. vyse)
    int a = -1;

    for (int i = 0; i < sLen; i++){
        while (a > -1 && search[a+1] != s[i]){ // Neni shoda
            a = Parray[a];
        }
        if (search[a+1] == s[i]){ // Zvyseni pri shodnosti
            a++;
        }
        if (a == seaLen-1){ // Nalezeni shody
            free(Parray);
            return i - a;
        }
    }
    free(Parray);
    return -1;
} // Funkce find

char *sort(char *s)
{
    unsigned len = strlen(s);

    if (len != 1) {  // Rozdeleni na mensi stringy
        unsigned x = 0;
        unsigned y = 0;
        unsigned count = 0; // Pocitadlo prvku pole
        unsigned half = len / 2;
        unsigned top;

        char *left = substr(s, 0, half);
        char *right;


        if (len % 2 == 0) { // char *sudy X lichy pocet clenu
            right = substr(s, half, 0);
            top = half;
        } else {
            right = substr(s, half, 0);
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
            x++;
        }
        while (y < top){   // Doplneni zbyvajicich znaku
            s[count] = right[y];
            count++;
            y++;
        }
    }   // if (len != 1)
    return s;
}   // function


void table_insert(SymbolTable *tree, Node *object){
  Node *next = NULL;
  Node *last= NULL;

  /* root does not exist */
  if (!tree->root){
    tree = createSymbolTable();
    tree->root = object;
  }
  /* root exists. Put it on next node */
  else{
    next = tree->root;

    while(next){
      last = next;
      int compare = strcmp(object->symbol, next->symbol);
      if (compare < 0)
        next = next->left;
      else if (compare > 0)
        next = next->right;
      else
        PERROR("Cannot insert same Node into table");
    }

    /* object not contain function create new Value node */
    Node *new;
    if ( !object->data.function )
      new = createValueNode(object->symbol, object->data.value);
    else
      new = createFunctionNode(object->symbol, object->data.function);

    int compare = strcmp(new->symbol, last->symbol);
    if (compare < 0)
      last->left = new;
    if (compare > 0)
      last->right = new;
  }
  //TODO balance tree
}

Node *table_lookup(SymbolTable *tree, Node *object){
  Node *current = tree->root;
  int compare = strcmp (current->symbol, object->symbol);

  /* loop ends at end or when comapre is eq 0 */
  while( current && compare ){
    /* comparison between 2 keys is bigger go right */
    if (compare > 0)
      current = current->right;
    /* comparison is less go left */
    else
      current = current->left;
  }
  /* returns NULL or looked object */
  return current;
}
