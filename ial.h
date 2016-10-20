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
#include "error.h"
#include "ir.h"

typedef enum {
    N_VALUE,
    N_FUNCTION,
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

/**
  * Seach for a Node in AVL table
  * returns pointer to Node
  */
Node *table_lookup(SymbolTable *, char *);

void pre_order(Node *);
void table_iterate(SymbolTable *tree, void (*fn)(Node *));

#endif /* IFJ_IAL_H */
