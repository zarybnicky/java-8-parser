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


/* Right left rotation of object */
Node *table_rotate_right_left (Node *object){
  Node *current = object;
  Node *right = current->right;
  Node *right_left = right->left;

  /* current right rotate to right left */
  current->right = right_left->left;
  /* right right left assign to right left right */
  right->left = right_left->right;

  /* new rotated right_left Node assign and return*/
  right_left->left = current;
  right_left->right = right;

  return right_left;
}

/* Left right rotation of object */
Node *table_rotate_left_right (Node *object){
  Node *current = object;
  Node *left = current->left;
  Node *left_right = left->right;

  /* current left assign to left right right */
  current->left = left_right->right;
  /* left right assign to left right left */
  left->right = left_right->left;

  /* new rotated left_right Node assign and return*/
  left_right->left = left;
  left_right->right = current;

  return left_right;
}

/* left left rotation of object */
Node *table_rotate_left_left (Node *object){
  Node *current = object;
  Node *left = current->left;

  /* change left with left right */
  current->left = left->right;
  left->right = current;

  return left;
}

/* right right rotation of object */
Node *table_rotate_right_right (Node *object){
  Node *current = object;
  Node *right = current->right;

  /* change right with right left */
  current->right = right->left;
  right->left = current;

  return right;
}

/* END OF ROTATION */
/* Compute geight of an AVL node recursively */
int table_node_height(Node * object){
  if (!object)
    return 0;
  int height_left = table_node_height(object->left);
  int height_right = table_node_height(object->right);

  /*  returns increment of height right if it is bigger else increment of
      height left*/
  return (height_right > height_left) ? ++height_right : ++height_left;
}

/* Balance factor of an AVL node */
int table_balance_factor (Node * object){
  int balance_factor = 0;

  /* left height is positive */
  if (object->left)
    balance_factor += table_node_height(object->left);
  /* right height is negative */
  if (object->right)
    balance_factor -= table_node_height(object->right);
  return balance_factor;
}

/**
  * Balance table based on factor of height.
  * Calling rotation function(s) to decrease height of tree
  */
Node *table_balance_node(Node *object){
  Node *new_root = NULL;

  if (object->left)
    object->left = table_balance_node (object->left);
  if (object->right)
    object->right = table_balance_node (object->right);

  int factor = table_balance_factor (object);

  if (factor >= 2){
    /*  factor is bigger or eq 2 which means that is Left heavy tree
        rotate leftright when factor of left is maximaly 1 right */
    if (table_balance_factor( object->left ) <= -1)
      new_root = table_rotate_left_right (object);

    /* heavier right balance rotate left left */
    else
      new_root = table_rotate_left_left (object);
  }
  else if (factor <= -2){
    /*  factor is lesser or eq 2 which means is Right heavy tree
        rotate rightleft when factor of right is minimaly 1 left */
    if (table_balance_factor( object->right ) >= 1)
      new_root = table_rotate_right_left(object);

    /* heavier right balance rotate right right */
    else
      new_root = table_rotate_right_right(object);
  }
  /* balance is maximaly 1 left 1 right or 0 (balanced node) */
  else
    new_root = object;
  return new_root;
}

/**
  * balance AVL tree if it is not balanced
  * when tree is balanced nothing changed
  */
void table_balance (SymbolTable *tree){
  Node *new_root = NULL;

  /* make new root from tree->root if it is not balanced */
  new_root = table_balance_node(tree->root);

  /* new root is not same as current, change root to new balanced root */
  if (new_root != tree->root)
    tree->root = new_root;
}

/**
  * insert object to table, insertion is made by comparing object key with
  * tree key. If 2 same keys PERROR occurs, at of funtion is called balance
  * function
  */
void table_insert(SymbolTable *tree, Node *object){
  Node *next = NULL;
  Node *last = NULL;

  if (tree == NULL)
    ERROR(ERR_RUNTIME_UNINITIALIZED);
  /* root does not exist */
  if (tree->root == NULL){
    tree->root = object;
    return;
  }
  /* root exists. Put it on next node */
  next = tree->root;

  while(next != NULL){
      last = next;
      int compare = strcmp(object->symbol, next->symbol);
      if (compare < 0)
          next = next->left;
      else if (compare > 0)
          next = next->right;
      else
          FERROR(ERR_SEM_UNDEFINED, "Cannot redefine object %s", object->symbol);
  }

  int compare = strcmp(object->symbol, last->symbol);
  if (compare < 0)
      last->left = object;
  if (compare > 0)
      last->right = object;

  /* balance tree after insertion */
  table_balance(tree);
}

Node *table_lookup(SymbolTable *tree, char *symbol){

  /* First chceck if table exists */
  if (tree == NULL)
    return NULL;
  /* if table does exists we can check root, cannot compare both options */
  if (tree->root == NULL)
    return NULL;

  Node *current = tree->root;
  int compare = strcmp (current->symbol, symbol);

  /* loop ends at end or when comapre is eq 0 */
  while( current && compare ){
    /* comparison between 2 keys is bigger go right */
    if (compare > 0)
      current = current->right;
    /* comparison is less go left */
    else
      current = current->left;
  }
  /* returns looked object */
  return current;
}

Node *createFunctionNode(char *symbol, Function *f) {
    Node *n = malloc(sizeof(Node));
    CHECK_ALLOC(n);
    n->symbol = symbol;
    n->type = N_FUNCTION;
    n->data.function = f;
    n->left = NULL;
    n->right = NULL;
    return n;
}
Node *createValueNode(char *symbol, Value *v) {
    Node *n = malloc(sizeof(Node));
    CHECK_ALLOC(n);
    n->symbol = symbol;
    n->type = N_VALUE;
    n->data.value = v;
    n->left = NULL;
    n->right = NULL;
    return n;
}
Node *createClassNode(char *symbol) {
    Node *n = malloc(sizeof(Node));
    CHECK_ALLOC(n);
    n->symbol = symbol;
    n->type = N_CLASS;
    n->left = NULL;
    n->right = NULL;
    return n;
}
void freeNode(Node *n) {
    if (n != NULL) {
        switch (n->type) {
        case N_VALUE:
            freeValue(n->data.value);
            break;
        case N_FUNCTION:
            freeFunction(n->data.function);
            break;
        case N_CLASS:
            break;
        }
        free(n->symbol);
        freeNode(n->left);
        freeNode(n->right);
        free(n);
    }
}
void printNode(Node *n)  {
    if (n == NULL) {
        printf("Node(NULL)");
        return; // ADD
    }
    printf("Node(%s, %s, ", n->symbol, showNodeType(n->type));
    printNode(n->left);
    printf(", ");
    printNode(n->right);
    printf(")\n");
}
char *showNodeType(NodeType x) {
    switch (x) {
    case N_VALUE:    return "Value";
    case N_FUNCTION: return "Function";
    case N_CLASS:    return "Class";
    }
    return "Unknown NodeType"; //Just to pacify the compiler...
}

/* SYMBOL TABLE */
SymbolTable *createSymbolTable()  {
    SymbolTable *t = malloc(sizeof(SymbolTable));
    CHECK_ALLOC(t);
    t->root = NULL;
    return t;
}
void freeSymbolTable(SymbolTable *t)  {
    if (t != NULL) {
        freeNode(t->root);
        free(t);
    }
}
void printSymbolTable(SymbolTable *t) {
    if (t != NULL) {
        printf("SymbolTable(");
        printNode(t->root);
        printf(")\n");
    }
}

/**
  * fn calling to iterate over every object in table TODO, no tree calling just
  * object better is to call tree and traverse table by it, we can hit object
  * with not left, right node
  */
void table_iterate(Node *object, void (*fn)(Node *object)){
  if (fn == NULL)
    return;
  if (object->left != NULL)
    table_iterate(object->left,fn);
  if (object->right != NULL)
    table_iterate(object->right,fn);

  fn (object);
}
