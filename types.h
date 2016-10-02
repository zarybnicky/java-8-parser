/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_TYPES_H
#define IFJ_TYPES_H

typedef char* String; //why?!

typedef struct {
  //??
} sym_table;

typedef struct {
  //??
} symbol;

enum symbol_obj_type { NODE_FN, NODE_VAR };
typedef struct {
  enum symbol_obj_type type;
  union {
    //??
  };
} symbol_obj;

#endif /* IFJ_TYPES_H */
