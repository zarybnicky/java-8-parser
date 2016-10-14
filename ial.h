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

#include "error.h"

char *sort(char *);
int find(char *haystack, char *needle);

/**
  * Create a new symbol table
  * returns new symbol table
  */
//glb_sym_table *create_symbol_table();

/** Create new symbol if not enough
  * memory exit ERR_INTERNAL returned
  */
//symbol *create_symbol(String, Value);

/**
  * Inserts symbol into created symbol table
  * TODO return int value for success??
  */
//void insert_symbol (glb_sym_table *, String, Value);
//void insert_function (glb_sym_table *, String, Value);
//void insert_to_local (lcl_ident_table *, String);

/**
  * Seach for a symbol in symbol table
  * returns symbol
  */
//symbol *lookup_symbol(glb_sym_table *, String);
//symbol *lookup_function (glb_sym_table *, String);
//symbol *lookup_in_local (lcl_ident_table *, String);

/**
  * Deletes symbol/function in global table ,
  * Variables or parameters in local table
  */
//void delete_symbol (glb_sym_table *);
//void delete_function (glb_sym_table *);
//void delete_in_local (lcl_ident_table *);

#endif /* IFJ_IAL_H */
