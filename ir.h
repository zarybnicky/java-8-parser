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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "int_memory_management.h"

typedef enum {
    T_STRING,
    T_INTEGER,
    T_DOUBLE,
    T_BOOLEAN,
    T_VOID,
} ValueType;

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
    C_DO_WHILE,
    C_FOR,
    C_EXPRESSION,
    C_CONTINUE,
    C_BREAK,
    C_RETURN,
} CommandType;

typedef struct tValue {
    ValueType type;
    bool undefined;
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
        struct { Block bodyBlock; Expression *cond; } doWhileC;
        struct {
            Declaration var; Expression *initial;
            Expression *cond; struct tCommand *iter;
            Block bodyBlock;
        } forC;
        Expression *expr; //C_EXPRESSION + C_RETURN
    } data;
} Command;

typedef struct {
    char *name;
    ValueType returnType;
    int argCount;
    Declaration *argHead;
    Block body;
    bool builtin;
} Function;

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

Command *createCommandDeclare(Declaration);
Command *createCommandDefine(Declaration, Expression *);
Command *createCommandAssign(char *, Expression *);
Command *createCommandBlock();
Command *createCommandIf(Expression *);
Command *createCommandWhile(Expression *);
Command *createCommandDoWhile();
Command *createCommandFor(Declaration, Expression *, Expression *, Command *);
Command *createCommandExpression(Expression *);
Command *createCommandContinue();
Command *createCommandBreak();
Command *createCommandReturn(Expression *);
void freeCommand(Command *);
void printCommand(Command *);

Function *createFunction(char *, ValueType, int, Declaration *);
void freeFunction(Function *);
void printFunction(Function *);
void traverseCommands(Function *, void (*f)(Function *, Command *), void (*onForExit)(Command *));

char *showValueType(ValueType);
char *showExpressionType(ExpressionType);
char *showBinaryOperation(BinaryOperation);
char *showCommandType(CommandType);

//IR.H
char *getReferenceName(char *);
char *getClassName(char *);
char *getFunctionName(char*);

#endif /* IFJ_IR_H */
