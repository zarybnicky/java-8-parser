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
#include "stringology.h"

int *Prefixcreator(char *search, int seaLen){ // Pomocna funkce pro find

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

int find(char *s, char *search) {

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

// Pointer to last inserted funciton
//local_function_table *last_function = NULL;


/*  Symbol table implementation global, local,
    pushing functions to global table*/
/*
glb_sym_table *create_symbol_table(){

  glb_sym_table *g_table = NULL;
  g_table = malloc(sizeof(glb_sym_table));
  if ( !g_table ){
    perror("Table allocation failed");
    exit(ERR_INTERNAL);
  }
  g_table->key = NULL;
  g_table->left = NULL;
  g_table->right = NULL;
  g_table->object = NULL;
  g_table->f_table = NULL;

  return g_table;
}
*/
/* Allocation of new symbol with key and data */
/*
symbol *create_symbol (char *key, Value data){
  symbol *new = malloc(sizeof(symbol));
  if ( !new ){
    perror("Allocation of symbol failed");
    exit(ERR_INTERNAL);
  }
  strcpy(new->key, key);
  new->data = data;
  new->left = NULL;
  new->right = NULL;

  return new;
}
*/
/** Insert symbol with its contents and key
  * comparing root key with new key
  */
/*
void insert_symbol(glb_sym_table *root, char *key, Value data){

  if ( !root ){
    root = create_symbol_table();
    symbol *sym = create_symbol(key, data);
    //key is name of object
    root->key = key;
    //insert object to root
    root->object = sym;
  }
  else
  {
    int cmp = strcmp(key, root->key);
    // if key is smaller
    if (cmp < 0)
      insert_symbol(root->left, key, data);
    // if key is bigger
    else if (cmp > 0)
      insert_symbol(root->right, key, data);
    // if key is equal push it to right and increment key
    else
    // ?!? symbol should be inserted even its key is same as the root->key
      insert_symbol(root->right, key + 1, data);
  }
}
*/
/* TODO function dont have same data as symbol does */
/*
void insert_function (glb_sym_table *root, char *key, Value data){
  if (!root){
    root = create_symbol_table();
    local_function_table *l_table = (local_function_table *) 1; //FIXME
    // TODO allocate function parameters
    l_table->params = 0;
    l_table->vars = 0;
    l_table->left = NULL;
    l_table->right = NULL;
    root->f_table = l_table;
    // key is name of function
    root->key = key;
    // last function is inserted one
    last_function = (local_function_table *) root; //unsafe!
  }
  else{
    // comparing key with local_table object
      int cmp = strcmp(key, root->key);
    if (cmp < 0)
      insert_function(root->left, key, data);
    else if (cmp > 0)
      insert_function(root->right, key, data);
    //key is same as r->key, which means that function is already inside BT
    else{
      perror("Cannot insert function with same key");
      exit(ERR_INTERNAL);
    }
  }
}
*/
/*
void insert_identifier (lcl_ident_table *root, char *key){
  if ( !root ){
    lcl_ident_table *l_table = malloc(sizeof(lcl_ident_table));
    if ( !l_table ){
      perror("Allocation of local symbol table failed");
      exit(ERR_INTERNAL);
    }
    l_table->key = key;
    //offset to recount better position in identifier BT
    l_table->offset = last_function->params + last_function->vars;
    l_table->left = NULL;
    l_table->right = NULL;
    root = l_table;
  }
  else{
    int cmp = strcmp(key, root->key);
    if (cmp < 0)
        ; //insert_to_local(root->left, key);
    else if (cmp > 0)
        ; //insert_to_local(root->right, key);
    else{
      perror("Cannot insert identifier with same key");
      exit(ERR_INTERNAL);
    }
  }
}

symbol *lookup_symbol(glb_sym_table *root, char *key){
  if ( !root ){
    return NULL;
  }
  int cmp = strcmp(key, root->object->key);

  // lookup key is different than root key
  if (cmp < 0)
    return lookup_symbol(root->left, key);
  else
    return lookup_symbol(root->right, key);

  //lookup key is same as root key. We finished lookup of symbol
  return root->object;
}

symbol *lookup_function (glb_sym_table *,String);
symbol *lookup_in_local (lcl_ident_table*, String);

void delete_symbol (glb_sym_table *);
void delete_function (glb_sym_table *);
void delete_in_local (lcl_ident_table *);
*/
/* TODO Insert function to glb_sym_table
   Insert param/variable to local_sym_table */

/*  TODO Lookup_function in glb_sym_table
    Lookup_symbol in glb_sym_table
    Lookup_symbol in local_sym_table */
