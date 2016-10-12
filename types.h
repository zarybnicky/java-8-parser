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

typedef enum {
    T_STRING,
    T_INTEGER,
    T_DOUBLE,
    T_BOOLEAN,
    T_VOID,
} ValueType;

typedef struct{
  String sym_name;          // symbol name
  ValueType type;
  union {
      int int_val;              //INTEGER val
      double dbl_val;           //DOUBLE val
      bool boolean_val;         //BOOLEAN val
      String str;               //STRING val
      void *adr;                //VOID val
  };
} Content;

/* Symbol defined */
typedef struct ptr{
  String key;                   // symbol contains key
  Content data;                 // symbol contains data
  struct ptr *left, *right;     // left right node of symbol
} symbol;

/* Local symbol table for functions constains function symbols */
typedef struct l_sym_table{
  String key;
  int params, vars;         // number of parameters and variables
  symbol *object;           // symbol with defined type,length and name
  struct l_sym_table *left, *right;
}local_function_table;

typedef struct l_iden_table{
  String key;
  int offset;
  struct l_iden_table *left, *right;
}lcl_ident_table;

typedef struct {} local_sym_table;

/* Global symbol table  contains local tables and global objects */
typedef struct g_sym_table{
  String key;                   // key for addressing in BST
  lcl_ident_table *f_table;     // pointer to a function table
  symbol *object;               // symbol with defined type,length and name
  struct g_sym_table *left, *right;
} glb_sym_table;

#endif /* IFJ_TYPES_H */
