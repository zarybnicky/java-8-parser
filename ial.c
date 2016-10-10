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

char *substr(char *s, unsigned start, unsigned length)
{
    return "";
}

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

  g_table = (glb_sym_table) malloc(sizeof(glb_sym_table));

  if (g_table == NULL){
    perror("Table allocation failed");
    exit(ERR_INTERNAL);
  }
  g_table->left = NULL;
  g_table->right = NULL;

  return g_table;
}

symbol *create_symbol (Content data){
  symbol *new = malloc(sizeof(symbol));
  if (new == NULL){
    perror("Allocation of symbol failed");
    exit(ERR_INTERNAL);
  }
  new->data = data;
  new->ptr->left = NULL;
  new->ptr->right = NULL;

  return new;
}

void insert_symbol(glb_sym_table *root, int key, Content data){
  if(!root){
    root = create_symbol_table();
    symbol *sym = create_symbol(data);
    root->;

  }
  // if key is smaller
  if (key < root->key)
    insert_symbol(root->left, key, sym);
  // if key is bigger
  else if (key > root->key)
    insert_symbol(root->right, key, sym);
  // if key is equal push it to right and increment key
  else
    insert_symbol(root->right, key + 1, sym);
}

symbol *lookup_symbol(global_sym_table *root, int key){

}

