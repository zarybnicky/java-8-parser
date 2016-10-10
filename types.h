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


/* Symbol defined */
typedef struct {
  String sym_name;  // symbol name
  String sym_type;  // type of symbol (boolean, double, int, void, String)
  int sym_length;   // max length of sym_type, for String based on strlen
} symbol;

/* Local symbol table for functions */
typedef struct l_sym_table{
  String name;
  int offset;
  symbol object;           // symbol with defined type,length and name
  String scope;            // scope (function parameter, global, local, static)
  enum symbol_obj_type type;
}local_sym_table;

/* Global symbol table */
typedef struct {
  String name;              // ?!?
  int key;                  // key for addressing in BST
  local_sym_table *f_table; // pointer to a function table
  symbol object;            // symbol with defined type,length and name
  bool declaration;         // symbol is declared == T
  bool definition;          // symbol is defined == T
  enum symbol_obj_type type;//
  
  /* TODO More or less information ??*/

} global_sym_table;


/* symbol_obj ??*/
/*  typedef struct {
    enum symbol_obj_type type;
    union {
      //??
    };
  } symbol_obj;
  */

#endif /* IFJ_TYPES_H */
