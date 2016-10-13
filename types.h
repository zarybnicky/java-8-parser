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

typedef enum {
    N_VALUE,
    N_FUNCTION,
} NodeType;

typedef enum {
    E_FUNCALL,
    E_VALUE,
    E_REFERENCE,
    E_BINARY,
} ExpressionType;

typedef enum {
    EB_EQUAL,
    EB_NOT_EQUAL,
    EB_LESS,
    EB_LESS_EQUAL,
    EB_GREATER,
    EB_GREATER_EQUAL,
    EB_MULTIPLY,
    EB_DIVIDE,
    EB_ADD,
    EB_SUBTRACT,
} BinaryOperation;

typedef enum {
    C_DECLARE,
    C_DEFINE,
    C_ASSIGN,
    C_BLOCK,
    C_IF,
    C_WHILE,
    C_EXPRESSION,
    C_RETURN,
} CommandType;

typedef struct tValue {
    ValueType type;
    union {
        int integer;
        double dbl;
        bool boolean;
        char *str;
    } data;
} Value;

typedef struct tExpression {
    ExpressionType type;
    struct tExpression *next; //for argument lists
    union {
        struct {
            char *name;
            int argCount;
            struct tExpression *argHead;
        } funcall;
        Value *value;
        char *reference;
        struct {
            BinaryOperation op;
            struct tExpression *left;
            struct tExpression *right;
        } binary;
    } data;
} Expression;

typedef struct tDeclaration {
    ValueType type;
    char *name;
    struct tDeclaration *next; //for argument lists
} Declaration;

struct tCommand;
typedef struct tBlock {
    struct tCommand *head;
    struct tCommand *tail;
} Block;

typedef struct tCommand {
    CommandType type;
    struct tCommand *next;
    union {
        Declaration declare;
        struct { Declaration declaration; Expression *expr; } define;
        struct { char *name; Expression *expr; } assign;
        Block block;
        struct { Expression *cond; Block thenBlock; Block elseBlock; } ifC;
        struct { Expression *cond; Block bodyBlock; } whileC;
        Expression *expr; //C_EXPRESSION + C_RETURN
    } data;
} Command;

typedef struct {
    char *name;
    int argCount;
    Declaration *argHead;
    Block body;
} Function;

typedef struct {
    char *name;
    NodeType type;
    union {
        Value *value;
        Function *function;
    } data;
} Node;

/* Symbol defined */
typedef struct ptr{
  String key;                   // symbol contains key
  Value data;                 // symbol contains data
  struct ptr *left, *right;     // left right node of symbol
} symbol;

/* Local symbol table for functions constains function symbols */
typedef struct l_function_table{
  String key;
  int params, vars;         // number of parameters and variables
  symbol *object;           // symbol with defined type,length and name
  struct l_function_table *left, *right;
}local_function_table;

typedef struct l_iden_table{
  String key;
  int offset;
  struct l_iden_table *left, *right;
}lcl_ident_table;

typedef struct l_sym_table {
  String key;
  struct l_sym_table *left, *right;
} local_sym_table;

/* Global symbol table  contains local tables and global objects */
typedef struct g_sym_table{
  String key;                   // key for addressing in BST
  local_function_table *f_table;     // pointer to a function table
  symbol *object;               // symbol with defined type,length and name
  struct g_sym_table *left, *right;
} glb_sym_table;

#endif /* IFJ_TYPES_H */
