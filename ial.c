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
#include "Stringology.h"

int *Prefixcreator(String search, int seaLen){ // Pomocna funkce pro find

    int *array = malloc(sizeof(int)*seaLen); // Prefixove pole
    if (array == NULL){
        perror("Error while allocating memory");
        exit(ERR_INTERNAL);
    }

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

int find(String s, String search) {
    
    int sLen = strlen(s);
    int seaLen = strlen(search);

    if (seaLen == 0){
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

String sort(String s)
{
    unsigned len = strlen(s);

    if (len != 1) {  // Rozdeleni na mensi stringy
        unsigned x = 0;
        unsigned y = 0;
        unsigned count = 0; // Pocitadlo prvku pole
        unsigned half = len / 2;
        unsigned top;

        String left = substr(s, 0, half);
        String right;


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

/*  Symbol table implementation global, local,
    pushing functions to global table*/

glb_sym_table *create_symbol_table(){
  glb_sym_table *g_table = NULL;

  g_table = malloc(sizeof(glb_sym_table));

  if (g_table == NULL){
    perror("Table allocation failed");
    exit(ERR_INTERNAL);
  }
  g_table->key = 0;
  g_table->left = NULL;
  g_table->right = NULL;
  g_table->object = NULL;
  g_table->f_table = NULL;

  return g_table;
}

symbol *create_symbol (String key, Content data){
  symbol *new = malloc(sizeof(symbol));
  if (new == NULL){
    perror("Allocation of symbol failed");
    exit(ERR_INTERNAL);
  }
  strcpy(new->key, key);
  new->data = data;
  new->left = NULL;
  new->right = NULL;

  return new;
}

void insert_symbol(glb_sym_table *root, String key, Content data){

  if(!root){
    root = create_symbol_table();
    symbol *sym = create_symbol(key, data);
    root->object = sym;

  }
  else
  {
    int cmp = strcmp(key, root->object->key);
    // if key is smaller
    if (cmp < 0)
      insert_symbol(root->left, key, data);
    // if key is bigger
    else if (cmp > 0)
      insert_symbol(root->right, key, data);
    // if key is equal push it to right and increment key
    else
      insert_symbol(root->right, key + 1, data);
  }
}

//symbol *lookup_symbol(glb_sym_table *root, String key){
//
//}

