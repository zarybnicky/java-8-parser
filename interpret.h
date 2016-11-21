/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef INTERPRET_H
#define INTERPRET_H



/*--------------------------------STRUCTURES--------------------------------*/

typedef struct {
    SymbolTable symTable;
} Interpret;

typedef struct Stack{
    struct Stack *prev;
    int cap;
    int size;
    Value *data[];
} Stack;

typedef enum{
    FN_PRINT,
    FN_READINT,
    FN_READDBL,
    FN_READSTR,
    FN_LENGTH,
    FN_SUBSTR,
    FN_COMPARE,
    FN_SORT,
    FN_FIND,
    FN_OTHER
} FnTypes;

/*--------------------------------VARIABLES---------------------------------*/



/*---------------------------------MACROS-----------------------------------*/



/*-------------------------------Declarations--------------------------------*/


Interpret *createInterpret(void);

int freeInterpret(Interpret *);


int evalMain(Interpret *);

//Static variables can be initialiezd using an expression as well, though I
//suppose these expressions shouldn't contain any references to variables?

int interpretNode(Stack *stack, Node *node);
int interpretFunc(Stack *stack, Node *node);
int evalCommand(SymbolTable *symTable, Stack *stack, Command *cmd);
int builtInFunc(SymbolTable *symTable, Stack *stack, Function *fn);
int pushParamsToStack(SymbolTable *symTable, Stack *stack, Function *fn);

Value *evalStaticExpression(Expression *);

Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right);

ValueType evalReturnType( BinaryOperation op, Value *left, Value *right);

char *str_cat(char *str1, char* str2);

Stack *createLocalStack(Stack *stack);

Stack *deleteLocaleStack(Stack *stack);

int pushToStack(Stack *stack, Value *val);

Value *popFromStack(Stack *stack);

Value *reTypeFromDeclToVal(Declaration *dec);

#endif
