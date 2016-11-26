/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_IAL_H
#define IFJ_IAL_H

#include "stringology.h"
#include "scanner.h"
#include "int_memory_management.h"

typedef enum {
    N_VALUE,
    N_FUNCTION,
    N_CLASS,
} NodeType;

typedef struct tNode {
    char *symbol;
    NodeType type;
    union {
        Value *value;
        Function *function;
    } data;
    struct tNode *left, *right;
} Node;

typedef struct {
    Node *root;
} SymbolTable;

Node *createFunctionNode(char *, Function *);
Node *createValueNode(char *, Value *);
Node *createClassNode(char *);
void freeNode(Node *);
void printNode(Node *);
char *showNodeType(NodeType);

SymbolTable *createSymbolTable();
void freeSymbolTable(SymbolTable *);
void printSymbolTable(SymbolTable *);

int *Prefixcreator( char *, int );
char *sort( char * );
int find( char *, char * );

/**
  * Rotation of tree left right , left left
  * right left and right right for balance of AVL tree
  */
Node *table_rotate_left_right ( Node * );
Node *table_rotate_left_left ( Node* );
Node *table_rotate_right_left ( Node * );
Node *table_rotate_right_right ( Node* );

/**
  * count factor of tree based on tree height
  */
int table_node_height( Node * );
int table_balance_factor ( Node * );

/**
  * balance Node in AVL table
  * returns balanced node which should be
  * inserted in balanced tree
  */
Node *table_balance_node( Node * );

/**
  * Balance AVL tree after for insertion function
  */
void table_balance( SymbolTable * );

/**
  * Inserts Node into created AVL table
  */
void table_insert( SymbolTable *, Node * );
void table_insert_dummy(SymbolTable *, Declaration);
void table_insert_function(SymbolTable *, Function *);

/**
 * Removes the Node referenced by `symbol` and returns the deleted node.
 */
Node *table_remove(SymbolTable *, char *);

/**
  * Seach for a Node in AVL table
  * returns pointer to Node
  */
Node **table_lookup_ptr(SymbolTable *, char *);
Node *table_lookup(SymbolTable *, char *);
Node *table_lookup_either(SymbolTable *global, SymbolTable *local, char *class, char *var);

void table_iterate_fn(Node *, void (*fn)(Function *));

#endif /* IFJ_IAL_H */
