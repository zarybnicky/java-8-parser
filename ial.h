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

#include <stdlib.h>
#include <string.h>
#include "types.h"

String sort(String s);

int find(String s, String search);

sym_table *create_symbol_table();
void insert_symbol(sym_table *, symbol *, symbol_obj *);
symbol_obj lookup_symbol(sym_table *, symbol *);

#endif /* IFJ_IAL_H */
