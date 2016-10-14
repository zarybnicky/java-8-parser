/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_IR_H
#define IFJ_IR_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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

Value *createValue(ValueType);
void freeValue(Value *);
void printValue(Value *);

Expression *createExpression(ExpressionType);
void freeExpression(Expression *);
void printExpression(Expression *);

Declaration *createDeclaration(ValueType, char *);
void freeDeclaration(Declaration *);
void printDeclaration(Declaration *);

void appendToBlock(Block *, Command *);
void freeBlock(Block);
void printBlock(Block *);

Command *createCommand(CommandType);
void freeCommand(Command *);
void printCommand(Command *);

Function *createFunction(char *, int, Declaration *);
void freeFunction(Function *);
void printFunction(Function *);

Node *createFunctionNode(char *, Function *);
Node *createValueNode(char *, Value *);
void freeNode(Node *);
void printNode(Node *);

SymbolTable *createSymbolTable();
void freeSymbolTable(SymbolTable *);
void printSymbolTable(SymbolTable *);

void printValueType(ValueType);
void printNodeType(NodeType);
void printExpressionType(ExpressionType);
void printBinaryOperation(BinaryOperation);
void printCommandType(CommandType);

#endif /* IFJ_IR_H */
