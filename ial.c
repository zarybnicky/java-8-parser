/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "ial.h"

List *lStart;
List *lEnd;

int *Prefixcreator(char *search, int seaLen){ // Auxiliary function of find
    int *array = malloc(sizeof(int)*seaLen);
    CHECK_ALLOC(array);

    int a = -1;
    array[0] = a;

    for (int i = 1; i < seaLen; i++){
        while (a > -1 && search[a+1] != search[i]){ // Match not found
            a = array[a];
        }
        if (search[a+1] == search[i]){ // Match found -> increase
            a++;
        }
        array[i] = a; // Match position
    }
    return array;
}

int find(char *s, char *search) {
    int sLen = strlen(s);
    int seaLen = strlen(search);
    if (seaLen == 0) {
        return 0;
    }

    int *Parray = Prefixcreator(search, seaLen); // Call auxiliary function
    int a = -1;

    for (int i = 0; i < sLen; i++){
        while (a > -1 && search[a+1] != s[i]){ // Match not found
            a = Parray[a];
        }
        if (search[a+1] == s[i]){ // Match found -> increase
            a++;
        }
        if (a == seaLen-1){ // Match found
            free(Parray);
            return i - a;
        }
    }
    free(Parray);
    return -1;
}

// Macro for sort
#define newStart(first, second, counterF, counterS, pom, presun) do{    \
    if (first > second){                                        \
        if (second < counterF){     \
            if (presun == false)  {counterS = second; }  \
            else {counterS = second +1; }                                \
        } else {                                                \
            counterS = counterF;                                \
            pom = counterF - second;                            \
        }                                                       \
    } else {                                                    \
        if (second >= counterF){                                \
            counterS = second;                                  \
        } else {                                                \
            counterS = counterF;                                \
            pom = counterF - second;                            \
        }                                                       \
   }                                                            \
} while(0);

// List functions
void deleteFirst(){
    List *pom = lStart;
    lStart = lStart->next;
    free_c(pom);
}

void insertList(int val, int length){
    List *new = malloc(sizeof(struct tList));
    CHECK_ALLOC(new);

    if (lStart == NULL){
        new->next = NULL;
        lEnd = new;        
    } else {
        new->next = NULL;
        lEnd->next = new;  
        lEnd = new;
    }
    new->index = val;
    new->length = length;

    if (lStart == NULL){
        lStart = lEnd;
    }
}

// Preparation for sort
void makeComponents(char* s){
    int sLen = strlen(s); 
    int *array = malloc_c(sLen * sizeof(int));

    int length = 1;

    for (int i = 0; i < sLen; i++){

        if (s[i] < s[i+1]){
            array[i] = i + 1;
        } else {
            array[i] = 0;
        }
    }

    for (int i = 0; i<sLen; i++){
        if (array[i] != 0){
            length++;
        } else {
            insertList(i - (length - 1), length);
            length = 1; 
        }
    }
    free_c(array);
}

char *sort(char *s)
{
    if (*s == '\0')
        return s;
    char *newString = strdup_(s);

    unsigned len = strlen(newString);

    makeComponents(newString);
    unsigned first, second, counterF, counterS, pom, lengthF, lengthS;
    char tmp;
    bool posun = false;
    bool presun = false;

    while (lStart != lEnd) {
        first = lStart->index;
        lengthF = lStart->length;
        deleteFirst();

        second = lStart->index;
        lengthS = lStart->length;
        deleteFirst();

        counterF = first;
        counterS = second;
        pom = 0;
        printf("first %d\n", counterF);
        printf("second %d\n", counterS);

        while (counterF != second + lengthS - 1){ // Sort in lists
            while (pom != second + lengthS){
                printf("%d %d\n", counterF, counterS);
                if (newString[counterF] > newString[counterS]){
                    tmp = newString[counterS];
                    newString[counterS] = newString[counterF];
                    newString[counterF] = tmp;
                     printf("%s\n", newString);
                } 

                if (posun == false && counterS >= first + lengthF){
                    if (lengthS == 1){
                        pom++;
                    } else {
                        newStart(first, second, counterF, counterS, pom, presun);
                        posun = true;
                    }
                } else {
                    counterS++;
                    pom++;
                }

                if (counterS == len){
                    if (counterS < second + lengthS){
                        counterS = 0;
                    }
                }
                if (pom == lengthS) break;
            }
            counterF++;
           // if (counterF == lengthF) break;

            if (counterF == first + lengthF){
                counterF = second;
                printf("jo\n");
            }
            if (second < first && counterF == second) presun = true;
            if (counterF == len){
                posun = false;
                break;
            }
            newStart(first, second, counterF, counterS, pom, presun);
            pom = 1;
            posun = false;
        }
            insertList(first, lengthF + lengthS);
    }

// Last sorting for non-circle order
    if (lStart->index != 0){
        counterF = lStart->index;
        tmp = newString[lStart->index];
        for (unsigned i = lStart->index; i != len + 1; i++){
            posun = false;

            while (i - 1 != counterF){
                newString[counterF] = newString[counterF - 1];
                counterF--;
                if (counterF == 0){
                    newString[counterF] = newString[len - 1];    
                    counterF = len - 1;
                }

            }
        }
        newString[0] = tmp;
    }

    return newString;
}


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
        else {
            fprintf(stderr, "Cannot redefine object %s\n", object->symbol);
            freeNode(object);
            ERROR(ERR_SEM_UNDEFINED);
        }
    }

    int compare = strcmp(object->symbol, last->symbol);
    if (compare < 0)
        last->left = object;
    if (compare > 0)
        last->right = object;

    /* balance tree after insertion */
    table_balance(tree);
}
void table_insert_dummy(SymbolTable *t, Declaration var) {
    Value *v = createValue(var.type);
    v->undefined = true;
    if (v->type == T_STRING)
        v->data.str = NULL;

    table_insert(t, createValueNode(strdup_(var.name), v));
}
void table_insert_function(SymbolTable *t, Function *f) {
    table_insert(t, createFunctionNode(strdup_(f->name), f));
}

Node **table_lookup_ptr(SymbolTable *tree, char *symbol) {
    if (tree == NULL || tree->root == NULL)
        return NULL;

    Node **current = &tree->root;
    while (*current != NULL) {
        int cmp = strcmp(symbol, (*current)->symbol);
        if (cmp > 0) {
            current = &(*current)->right;
        } else if (cmp < 0) {
            current = &(*current)->left;
        } else {
            break;
        }
    }
    if (*current == NULL)
        return NULL;
    return current;
}
Node *table_lookup(SymbolTable *tree, char *symbol) {
    Node **n = table_lookup_ptr(tree, symbol);
    if (n == NULL)
        return NULL;
    return *n;
}

Node *table_lookup_either(SymbolTable *global, SymbolTable *local, char *class, char *var) {
    Node *n = table_lookup(local, var);
    if (n != NULL) {
        return n;
    }

    /* check global local with changed name */
    if (NULL != strchr(var, '.')) {
        n = table_lookup(global, var);
        return n;
    }


    int classLength = strlen(class);
    int idLength = strlen(var);
    char *qualified = malloc((classLength + 1 + idLength + 1));
    CHECK_ALLOC(qualified);
    strcpy(qualified, class);
    qualified[classLength] = '.';
    strcpy(qualified + classLength + 1, var);
    qualified[classLength + 1 + idLength] = '\0';

    n = table_lookup(global, qualified);
    free(qualified);
    return n;
}

Node *table_remove(SymbolTable *table, char *symbol){
    if (table == NULL || symbol == NULL) {
        MERROR(ERR_SEM_UNDEFINED, "Cannot remove without parameters.");
    }

    Node **localRootPtr = table_lookup_ptr(table, symbol);
    if (localRootPtr == NULL) {
        return NULL;
    }

    Node *deleted = *localRootPtr;

    //remove the node from the tree
    if (deleted->left == NULL) {
        *localRootPtr = deleted->right;
    } else if (deleted->right == NULL) {
        *localRootPtr = deleted->left;
    } else {
        //swap with the rightmost (grand)child of the left child
        Node **childPtr = &deleted->left;
        while ((*childPtr)->right != NULL) {
            childPtr = &(*childPtr)->right;
        }
        Node *child = *childPtr;
        //it can't have right children but may have left ones
        *childPtr = child->right;

        child->left = deleted->left;
        child->right = deleted->right;
        *localRootPtr = child;
    }

    deleted->left = deleted->right = NULL;
    return deleted;
}

Node *createFunctionNode(char *symbol, Function *f) {
    Node *n = malloc_c(sizeof(Node));
    n->symbol = symbol;
    n->type = N_FUNCTION;
    n->data.function = f;
    n->left = NULL;
    n->right = NULL;
    return n;
}
Node *createValueNode(char *symbol, Value *v) {
    Node *n = malloc_c(sizeof(Node));
    n->symbol = symbol;
    n->type = N_VALUE;
    n->data.value = v;
    n->left = NULL;
    n->right = NULL;
    return n;
}
Node *createClassNode(char *symbol) {
    Node *n = malloc_c(sizeof(Node));
    n->symbol = symbol;
    n->type = N_CLASS;
    n->left = NULL;
    n->right = NULL;
    return n;
}
void freeNode(Node *n) {
    if (n == NULL) {
        return;
    }
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
    free_c(n->symbol);
    freeNode(n->left);
    freeNode(n->right);
    free_c(n);
}
void printNode(Node *n)  {
    if (n == NULL) {
        printf("Node(NULL)\n");
        return;
    }
    switch (n->type) {
    case N_VALUE:
        printf("Node(%s, ", n->symbol);
        printValue(n->data.value);
        printf(", ");
        break;
    default:
        printf("Node(%s, %s, ", n->symbol, showNodeType(n->type));
        break;
    }
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
    SymbolTable *t = malloc_c(sizeof(SymbolTable));
    t->root = NULL;
    return t;
}
void freeSymbolTable(SymbolTable *t)  {
    if (t != NULL) {
        freeNode(t->root);
        free_c(t);
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
void table_iterate_fn(Node *object, void (*fn)(Function *)){
    if (fn == NULL)
        return;
    if (object->left != NULL)
        table_iterate_fn(object->left, fn);
    if (object->right != NULL)
        table_iterate_fn(object->right, fn);

    if (object->type == N_FUNCTION && !object->data.function->builtin)
        fn(object->data.function);
}
