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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ERR_LEXER                 1
#define ERR_SYNTAX                2
#define ERR_SEM_UNDEFINED         3
#define ERR_SEM_TYPECHECK         4
#define ERR_SEM_MISC              6
#define ERR_RUNTIME_INT_PARSE     7
#define ERR_RUNTIME_UNINITIALIZED 8
#define ERR_RUNTIME_DIV_BY_ZERO   9
#define ERR_RUNTIME_MISC          10
#define ERR_INTERNAL              99

typedef char* String; //why?!

enum symbol_obj_type { NODE_FN, NODE_VAR };

/* only 1 type of value can be assigned to 1 variable */
typedef union {
  int int_val;              //INTEGER val
  double dbl_val;           //DOUBLE val
  bool boolean_val;         //BOOLEAN val
  String str;               //STRING val
  void *adr;                //VOID val
}uniq_val;

/* length removed from symbol content */
typedef struct{
  String sym_name;          // symbol name
  int type;                 /*  type of symbol (boolean, double, int, void,
                                String) */
  uniq_val value;           //unique_value in which will be stored value
}Content;

/* Symbol defined */
typedef struct ptr{
  String key;                  // symbol contains key
  Content data;             // symbol contains data
  struct ptr *left, *right; // left right node of symbol
} symbol;

/* Local symbol table for functions constains function symbols */
typedef struct l_sym_table{
  String key;
  symbol *object;           // symbol with defined type,length and name
  enum symbol_obj_type type;
  struct l_sym_table *left, *right;
}local_sym_table;

/* Global symbol table  contains local tables and global objects */
typedef struct g_sym_table{
  String name;            // ?!?
  String key;                  // key for addressing in BST
  local_sym_table *f_table; // pointer to a function table
  symbol *object;            // symbol with defined type,length and name
  struct g_sym_table *left, *right;
  /* TODO More or less information
      String scope;            // scope (function parameter, global, local, static)?? */

} glb_sym_table;


/* symbol_obj ??*/
/*  typedef struct {
    enum symbol_obj_type type;
    union {
      //??
    };
  } symbol_obj;
  */

#endif /* IFJ_TYPES_H */
