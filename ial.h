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

int *Prefixcreator(char *, int );
char *sort(char *);
int find(char *, char *);


/**
  * Inserts Node into created AVL table
  */
void table_insert(SymbolTable *, Node *);

/**
  * Seach for a Node in AVL table
  * returns pointer to Node
  */
Node * table_lookup(SymbolTable *,Node *);




#endif /* IFJ_IAL_H */
