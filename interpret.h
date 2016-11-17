/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

/**
 *  @file interpret.h
 *  @version  0.1
 *  @author   xzaryb00 - Zarybnický Jakub
 *            xtamas01 - Tamaškovič Marek
 *            xvasko12 - Vaško Martin
 *            xvasko14 - Vaško Michal
 *            xzales12 - Záleský Jiří
 *  @date     12.11.2016
 *  @note
 */

/**
 * @defgroup interpret_structors Interpret Con-/De- structors
 * @brief    Interpret constructors and destructors.
 */

/**
 * @defgroup interpret_stack Interpret stack operations
 * @brief    Functions which handles operations over stack in interpret.
 */

#ifndef INTERPRET_H
#define INTERPRET_H



/*--------------------------------STRUCTURES--------------------------------*/

/**
    *   This typedef is
    *   @brief Structure which includes information about point.
    *   @typedef Interpret
    *   @var Interpret::symTable
    *   symTable is identificator for symbolic table
    *
    */

typedef struct {
    SymbolTable symTable;
} Interpret;

typedef struct Stack{
    struct Stack *prev;
    int cap;
    int size;
    Value *data[];
} Stack;

/*--------------------------------VARIABLES---------------------------------*/

// Some variables which shoul be global

/*---------------------------------MACROS-----------------------------------*/



/*-------------------------------DEFINITIONS--------------------------------*/


Interpret *createInterpret(void);
/**
 *  Creates table of symbols and initialize interpret
 *  @fn         Interpret *createInterpret(void)
 *  @brief      Creates interpret.
 *  @ingroup    interpret_structors
 *  @pre        none
 *  @post       none
 *  @return     pointer to Interpret [symbolic table]
 */

int freeInterpret(Interpret *);
/**
 *  Removes table of symbols and deletes interpret
 *  @fn         int freeInterpret(Interpret *)
 *  @brief      Frees interpret.
 *  @ingroup    interpret_structors
 *
 *  @param[IN]  Interpret* pointer to symbolic table
 *
 *  @pre        none
 *  @post       Interpret* can not be NULL pointer, Interpret*->symTable.root can not be NULL pointer
 *  @return     0 if success, nonzero in error
 */

int evalMain(Interpret *);

//Static variables can be initialiezd using an expression as well, though I
//suppose these expressions shouldn't contain any references to variables?

/**
 * @brief      Value *evalStaticExpression(Expression *)
 *
 * @param      <unnamed>  pointer to expression
 *
 * @return     value of expression
 */

Value *evalStaticExpression(Expression *);

/**
 * @brief      Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right)
 *
 * @param[in]  op     The operation
 * @param      left   The left
 * @param      right  The right
 *
 * @return     Value of expression
 */
Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right);

/**
 * @brief      ValueType evalReturnType( BinaryOperation op, Value *left, Value *right)
 *
 * @param[in]  op     The operation
 * @param      left   The left
 * @param      right  The right
 *
 * @return     Value of expression
 */
ValueType evalReturnType( BinaryOperation op, Value *left, Value *right);

/**
 * @brief      char *str_cat(char *str1, char* str2)
 *
 * @param      str1  The string 1
 * @param      str2  The string 2
 *
 * @post       str1 and str2 can not be NULL pointers.
 * @return     pointer to concatenated string
 */
char *str_cat(char *str1, char* str2);

/**
 * @brief      Creates a local stack.
 * @ingroup    interpret_stack
 *
 * @param      stack  The stack
 *
 * @post       stack can not be NULL pointer
 * @return     memory adress where is located stack
 */
Stack *createLocalStack(Stack *stack);


/**
 * @brief      deletes local stack.
 * @ingroup    interpret_stack
 *
 * @param      stack  The stack
 *
 * @post       stack can not be NULL pointer
 * @return     0 if success, nonzero in error
 */
Stack *deleteLocaleStack(Stack *stack);

/**
 * @brief      push value to stack.
 * @ingroup    interpret_stack
 *
 * @param      stack  The stack
 * @param      val    The value
 *
 * @post       stack can not be NULL pointer
 * @return     0 if success, nonzero in error
 */
int pustToStack(Stack *stack, Value *val);

/**
 * @brief      pop value from stack.
 * @ingroup    interpret_stack
 *
 * @param      stack  The stack
 *
 * @post       stack can not be NULL pointer
 * @return     pointer to value.
 */
Value *popFromStack(Stack *stack);

#endif
