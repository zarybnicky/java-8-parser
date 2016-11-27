/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */


#include "interpret.h"

#define TRUE true
#define FALSE false

static SymbolTable *symTableGlob = NULL;
static Stack *GlobalStack = NULL;

static bool continueFlag = FALSE;
static bool breakFlag = FALSE;
static bool returnFlag = FALSE;



#define FN(i, ret, fnName, count, arg) do {                             \
        char *name = strdup_("ifj16." #fnName);                         \
        Function *f = createFunction(name, ret, count, arg);            \
        f->builtin = true;                                              \
        table_insert_function(&i->symTable, f);                         \
    } while (0);

Interpret *createInterpret(void) {
    Interpret *i = malloc(sizeof(Interpret));
    CHECK_ALLOC(i);
    i->symTable.root = NULL;

    Declaration *printArg = createDeclaration(T_STRING, strdup_("x"));
    Declaration *sortArg = createDeclaration(T_STRING, strdup_("x"));
    Declaration *lengthArg = createDeclaration(T_STRING, strdup_("x"));

    Declaration *findArg1 = createDeclaration(T_STRING, strdup_("x"));
    Declaration *findArg2 = createDeclaration(T_STRING, strdup_("y"));
    findArg2->next = findArg1;
    Declaration *compareArg1 = createDeclaration(T_STRING, strdup_("x"));
    Declaration *compareArg2 = createDeclaration(T_STRING, strdup_("y"));
    compareArg2->next = compareArg1;
    Declaration *substrArg1 = createDeclaration(T_STRING, strdup_("x"));
    Declaration *substrArg2 = createDeclaration(T_INTEGER, strdup_("n"));
    Declaration *substrArg3 = createDeclaration(T_INTEGER, strdup_("l"));
    substrArg2->next = substrArg1;
    substrArg3->next = substrArg2;

    table_insert(&i->symTable, createClassNode(strdup_("ifj16")));

    FN(i, T_INTEGER, readInt, 0, NULL);
    FN(i, T_DOUBLE, readDouble, 0, NULL);
    FN(i, T_STRING, readString, 0, NULL);
    FN(i, T_VOID, print, 1, printArg);
    FN(i, T_STRING, sort, 1, sortArg);
    FN(i, T_INTEGER, length, 1, lengthArg);
    FN(i, T_INTEGER, find, 2, findArg2);
    FN(i, T_INTEGER, compare, 2, compareArg2);
    FN(i, T_STRING, substr, 3, substrArg3);

    return i;
}


int freeInterpret(Interpret *i) {
    if (i == NULL)
        return 1;
    if (i->symTable.root != NULL)
        freeNode(i->symTable.root);
    free(i);
    return 0;
}

int evalMain(Interpret *i) {
    assert(i != NULL);
    symTableGlob = &(i->symTable);
    GlobalStack = createLocalStack(NULL);

    Node *mainFn = table_lookup(&i->symTable, "Main.run");

    assert(mainFn != NULL);
    assert(mainFn->type == N_FUNCTION);

    interpretFunc(GlobalStack, mainFn);

    free(GlobalStack);
    return 0;
}

int interpretFunc(Stack *stack, Node *node) {

    // if is node
    Stack *localStack = NULL;
    SymbolTable *localTable = NULL;

    Node *mainFn = table_lookup(symTableGlob, "Main.run");

    if(strcmp(node->symbol, mainFn->symbol)){
        dPrintf("%s","Creating new local stack.");
        localStack = createLocalStack(stack);
    }
    else{
        dPrintf("%s\n\t- %s\n\t- %s","Setting reference of:","local stack to global stack,","local table to global table.");
        localStack = GlobalStack;
    }

    dPrintf("%s","Creating new local table.");
    localTable = createSymbolTable();
    ht_insert (&alloc_tab, localTable);

    assert(localStack != NULL);
    assert(localTable != NULL);

    Command *current = node->data.function->body.head;

    Command *tail = node->data.function->body.tail;

    for(;current != tail; current = current->next){
        evalCommand(localTable, localStack, current, getClassName(node->data.function->name));
    }
    evalCommand(localTable, localStack, current, getClassName(node->data.function->name));


    Node *n = table_lookup_either(symTableGlob, localTable, getClassName(mainFn->symbol),"b");
    printValue(n->data.value);


    return 0;
}

#define CYCLE_INNER(symTable, stack, funcName, body, end)       \
    do {                                                        \
        Command *c = body.head;                                 \
        while (c != NULL) {                                     \
            evalCommand(symTable, stack, c, funcName);          \
            if (breakFlag) {                                    \
                breakFlag = false;                              \
                goto end;                                       \
            }                                                   \
            if (continueFlag) {                                 \
                continueFlag = false;                           \
                break;                                          \
            }                                                   \
            c = c->next;                                        \
        }                                                       \
    } while (0);


Value *evalCommand(SymbolTable *symTable, Stack *stack, Command *cmd, char *className){

    Node *node;
    Value *val = NULL;

    continueFlag = FALSE;
    breakFlag = FALSE;
    returnFlag = FALSE;

    switch(cmd->type){
        case(C_DECLARE):
            //  insert declaration into table
            table_insert_dummy(symTable, cmd->data.declare);
            break;

        case(C_DEFINE):
            val = evalExpression(symTable, stack, className, cmd->data.define.expr);
            table_insert(symTable, createValueNode(cmd->data.define.declaration.name, val));
            break;

        case(C_ASSIGN):
            node = table_lookup_either(symTableGlob, symTable, className, cmd->data.assign.name);

            if(node == NULL)
                PERROR("Interpret: CMD: Assign: Variable not found in local or global symbol table.");

            val = evalExpression(symTable, stack, className, cmd->data.assign.expr);

            if(val == NULL)
                PERROR("Interpret: CMD: Assign: Evaluation of value was not successful.")

            // assign
            node->data.value = val;

            break;

        case(C_BLOCK):
            evalBlock(symTable, stack, &(cmd->data.block), className);
            break;

        case(C_IF):
            if (evalCondition(symTable, stack, className, cmd->data.ifC.cond)) {
                evalBlock(symTable, stack, &(cmd->data.ifC.thenBlock), className);
            } else {
                evalBlock(symTable, stack, &(cmd->data.ifC.elseBlock), className);
            }
            break;

        case(C_EXPRESSION):
            val = evalExpression(symTable, stack, className, cmd->data.expr);
            break;

        case(C_RETURN):
            val = evalExpression(symTable, stack, className, cmd->data.expr);
            pushToStack(GlobalStack, val);
            returnFlag = true;
            break;

        case(C_CONTINUE):
            continueFlag = true;
            break;

        case(C_BREAK):
            breakFlag = true;
            break;

        case(C_FOR):
            node = createValueNode(cmd->data.forC.var.name,
                                   evalExpression(symTable, stack, className, cmd->data.forC.initial));
            table_insert(symTable, node);

            while (evalCondition(symTable, stack, className, cmd->data.forC.cond)){
                CYCLE_INNER(symTable, stack, className, cmd->data.forC.bodyBlock, for_end);

                node->data.value = evalCommand(symTable, stack, cmd->data.forC.iter, className);
            }
    for_end:
            table_remove(symTable, cmd->data.forC.var.name);
            break;

        case(C_WHILE):
            while (evalCondition(symTable, stack, className, cmd->data.whileC.cond)) {
                CYCLE_INNER(symTable, stack, className, cmd->data.whileC.bodyBlock, while_end);
            }
    while_end:
            break;

        case(C_DO_WHILE):
            do {
                CYCLE_INNER(symTable, stack, className, cmd->data.doWhileC.bodyBlock, doWhile_end);
            } while (evalCondition(symTable, stack, className, cmd->data.doWhileC.cond));
    doWhile_end:
            break;
    }

    return val;
}

bool evalCondition(SymbolTable *symTable, Stack *stack, char *funcName, Expression *cond) {
    Value *val = evalExpression(symTable, stack, funcName, cond);
    if (val == NULL)
        MERROR(ERR_RUNTIME_MISC, "Interpret: CMD: evalCondition: Evaluation was unsuccessful.");
    if (val->type != T_BOOLEAN)
        MERROR(ERR_RUNTIME_MISC, "Interpret: CMD: evalCondition: Condition is not a boolean expression");
    return val->data.boolean;
}

int evalBlock(SymbolTable *symTable, Stack *stack, Block *block, char *className){
    Command *current = block->head;

    while (current != NULL) {
        if(continueFlag)
            continueFlag = FALSE;

        evalCommand(symTable, stack, current, className);

        if(continueFlag){
            current = block->head;
            break;
        }
        else if(breakFlag){
            break;
        }
        else if(returnFlag){
            break;
        }

        current = current->next;
    }
    return 0;
}

Value *evalFunction(Stack *localStack, SymbolTable* localSymTable, char *name, int argCount, Expression *argHead){
    (void) argCount;
    (void) argHead;

    Value *val = NULL;

    Node *node = table_lookup(symTableGlob, name);

    Function *fn = node->data.function;

    if(fn->builtin == TRUE){
        builtInFunc(localSymTable, localStack, node->data.function);
        return popFromStack(localStack);
    }

    val = createValue(fn->returnType);
    ht_insert(&alloc_tab,val);

    evalBlock(localSymTable, localStack, &(fn->body), fn->name);

    return val;
}

/**
 * Look for builtin functions
 */
int builtInFunc(SymbolTable *symTable, Stack *stack, Function *fn){
    (void) symTable;

    char *str = fn->name;

    if(!strcmp(str, "ifj16.print")){
        Value *term = popFromStack(stack);

        print(term);

        return 0;
    }
    else if(!strcmp(str, "ifj16.readInt") ){
        Value *val = createValue(T_INTEGER);
        val->data.integer = readInt();

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.readDouble") ){
        Value *val = createValue(T_DOUBLE);
        val->data.dbl = readDouble();

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.readString") ){
        Value *val = createValue(T_STRING);
        val->data.str = readString();

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.length") ){

        char *s = popFromStack(stack)->data.str;
        Value *val = createValue(T_INTEGER);
        val->data.integer = length(s);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.substr") ){
        char *s = popFromStack(stack)->data.str;
        int i = popFromStack(stack)->data.integer;
        int n = popFromStack(stack)->data.integer;

        Value *val = createValue(T_STRING);
        val->data.str = substr(s, i, n);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if (!strcmp(str, "ifj16.compare")) {
        char *s1 = popFromStack(stack)->data.str;
        char *s2 = popFromStack(stack)->data.str;

        dPrintf("s1: '%s',\n s2: '%s'", s1, s2);

        Value *val = createValue(T_INTEGER);
        val->data.integer = compare(s1, s2);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.sort") ){

        Value *val = popFromStack(stack);
        char *s = val->data.str;

        val->type = T_STRING;
        val->data.str = sort(s);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);

        return 0;
    }
    else if(!strcmp(str, "ifj16.find") ){

        Value *val = popFromStack(stack);
        char *s1 = val->data.str;

        val = popFromStack(stack);
        char *s2 = val->data.str;

        val->type = T_INTEGER;
        val->data.integer = find(s1, s2);

        stack->prev != NULL ? pushToStack(stack->prev, val) : pushToStack(stack, val);
        return 0;
    }
    else
        return -1;
}

int pushParamToStack(SymbolTable *symTable, Stack *stack, char* funcName, Expression *e) {
    pushToStack(stack, evalExpression(symTable, stack, funcName, e));
    return 0;
}

Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right) {

    Value *result = NULL;

    switch(left->type){
    case(T_INTEGER):
        switch(right->type){
        case(T_INTEGER):
            result = evalOperation(op,left,right);
            result->type = T_INTEGER;
            if(result->undefined == true){
                freeValue(result);
                result = evalBool(op, left, right);
            }
            return result;
        case(T_DOUBLE):
            result = evalOperation(op,left,right);
            result->type = T_DOUBLE;
            if(result->undefined == true){
                freeValue(result);
                result = evalBool(op, left, right);
            }
            return result;
        case(T_STRING):
        case(T_VOID):
        case(T_BOOLEAN):
            result = createValue(T_VOID);
            break;
        }
        break;
    case(T_DOUBLE):
        switch(right->type){
        case(T_INTEGER):
        case(T_DOUBLE):
            result = evalOperation(op,left,right);
            result->type = T_DOUBLE;
            if(result->undefined == true){
                freeValue(result);
                result = evalBool(op, left, right);
            }
            return result;
        case(T_STRING):
        case(T_VOID):
        case(T_BOOLEAN):
            result = createValue(T_VOID);
            break;
        }
        break;
    case(T_STRING):
        switch(right->type){
        case(T_STRING):
            result = evalOperation(op,left,right);
            result->type = T_STRING;
            if(result->undefined == true){
                freeValue(result);
                result = evalBool(op, left, right);
            }
            return result;
        case(T_INTEGER):
        case(T_DOUBLE):
        case(T_VOID):
        case(T_BOOLEAN):
            result = createValue(T_VOID);
            break;
        }
        break;
    case(T_VOID):
        result = createValue(T_VOID);
        break;
    case(T_BOOLEAN):
        switch(right->type){
        case(T_INTEGER):
        case(T_DOUBLE):
        case(T_STRING):
        case(T_VOID):
            result = createValue(T_VOID);
            break;
        case(T_BOOLEAN):
            result = evalBool(op,left,right);
            result->type = T_BOOLEAN;
            return result;
        }
        break;
    }

    result->undefined = false;

    return result;
}

Value *evalBool(BinaryOperation op, Value *left, Value *right){

    Value *result = createValue(T_BOOLEAN);


    switch(left->type){
    case(T_INTEGER):
        switch(right->type){
        case(T_INTEGER):
            switch(op){
                case(EB_EQUAL):
                    result->data.boolean=left->data.integer == right->data.integer;
                    return result;
                case(EB_NOT_EQUAL):
                    result->data.boolean=left->data.integer != right->data.integer;
                    return result;
                case(EB_LESS):
                    result->data.boolean=left->data.integer < right->data.integer;
                    return result;
                case(EB_LESS_EQUAL):
                    result->data.boolean=left->data.integer <= right->data.integer;
                    return result;
                case(EB_GREATER):
                    result->data.boolean=left->data.integer > right->data.integer;
                    return result;
                case(EB_GREATER_EQUAL):
                    result->data.boolean=left->data.integer >= right->data.integer;
                    return result;
                default:
                    break;
            }
            break;
        case(T_DOUBLE):
            switch(op){
                case(EB_EQUAL):
                    result->data.boolean=left->data.integer == right->data.dbl;
                    return result;
                case(EB_NOT_EQUAL):
                    result->data.boolean=left->data.integer != right->data.dbl;
                    return result;
                case(EB_LESS):
                    result->data.boolean=left->data.integer < right->data.dbl;
                    return result;
                case(EB_LESS_EQUAL):
                    result->data.boolean=left->data.integer <= right->data.dbl;
                    return result;
                case(EB_GREATER):
                    result->data.boolean=left->data.integer > right->data.dbl;
                    return result;
                case(EB_GREATER_EQUAL):
                    result->data.boolean=left->data.integer >= right->data.dbl;
                    return result;
                default:
                    break;
            }
            break;
        case(T_STRING):
        case(T_VOID):
        case(T_BOOLEAN):
            break;
        }
        break;
    case(T_DOUBLE):
        switch(right->type){
        case(T_INTEGER):
            switch(op){
                case(EB_EQUAL):
                    result->data.boolean=left->data.dbl == right->data.integer;
                    return result;
                case(EB_NOT_EQUAL):
                    result->data.boolean=left->data.dbl != right->data.integer;
                    return result;
                case(EB_LESS):
                    result->data.boolean=left->data.dbl < right->data.integer;
                    return result;
                case(EB_LESS_EQUAL):
                    result->data.boolean=left->data.dbl <= right->data.integer;
                    return result;
                case(EB_GREATER):
                    result->data.boolean=left->data.dbl > right->data.integer;
                    return result;
                case(EB_GREATER_EQUAL):
                    result->data.boolean=left->data.dbl >= right->data.integer;
                    return result;
                default:
                    break;
            }
            break;
        case(T_DOUBLE):
            switch(op){
                case(EB_EQUAL):
                    result->data.boolean=left->data.dbl == right->data.dbl;
                    return result;
                case(EB_NOT_EQUAL):
                    result->data.boolean=left->data.dbl != right->data.dbl;
                    return result;
                case(EB_LESS):
                    result->data.boolean=left->data.dbl < right->data.dbl;
                    return result;
                case(EB_LESS_EQUAL):
                    result->data.boolean=left->data.dbl <= right->data.dbl;
                    return result;
                case(EB_GREATER):
                    result->data.boolean=left->data.dbl > right->data.dbl;
                    return result;
                case(EB_GREATER_EQUAL):
                    result->data.boolean=left->data.dbl >= right->data.dbl;
                    return result;
                default:
                    break;
            }
            break;
        case(T_STRING):
        case(T_VOID):
        case(T_BOOLEAN):
            break;
        }
        break;
    case(T_STRING):
        switch(right->type){
        case(T_STRING):
            switch(op){
                case(EB_EQUAL):
                case(EB_NOT_EQUAL):
                case(EB_LESS):
                case(EB_GREATER):
                    result->data.boolean= strcmp(left->data.str,right->data.str);
                    return result;
                default:
                    break;
            }
            break;
        case(T_INTEGER):
        case(T_DOUBLE):
        case(T_VOID):
        case(T_BOOLEAN):
            break;
        }
        break;
    case(T_VOID):
        break;
    case(T_BOOLEAN):
        switch(right->type){
        case(T_INTEGER):
        case(T_DOUBLE):
        case(T_STRING):
        case(T_VOID):
            break;
        case(T_BOOLEAN):
            switch(op){
                case(EB_EQUAL):
                    result->data.boolean=left->data.boolean == right->data.boolean;
                    return result;
                case(EB_NOT_EQUAL):
                    result->data.boolean=left->data.boolean != right->data.boolean;
                    return result;
                case(EB_LESS):
                case(EB_LESS_EQUAL):
                case(EB_GREATER):
                case(EB_GREATER_EQUAL):
                    break;
                default:
                    break;
            }
            break;
        }
        break;
    }

    result->undefined = true;
    return result;

}

Value *evalOperation(BinaryOperation op, Value *left, Value *right){


    if(left->undefined || right->undefined)
        ERROR(ERR_RUNTIME_UNINITIALIZED);

    Value *result = createValue(evalReturnType(op,left,right));

    // TODO BOOLOP

    switch(op){
    case(EB_MULTIPLY):
        switch(result->type){
            case(T_INTEGER):
                result->data.integer = left->data.integer * right->data.integer;
                return result;
            case(T_DOUBLE):
                result->data.dbl = toDouble(left) * toDouble(right);
                return result;
            default:
            break;
        }
        break;
    case(EB_DIVIDE):
        switch(result->type){
            case(T_INTEGER):

                if(right->data.integer == 0)
                    ERROR(ERR_RUNTIME_DIV_BY_ZERO);

                result->data.integer = left->data.integer / right->data.integer;
                return result;

            case(T_DOUBLE):

                if(toDouble(right) == 0.0)
                    ERROR(ERR_RUNTIME_DIV_BY_ZERO);

                result->data.dbl = toDouble(left) / toDouble(right);
                return result;

            default:
            break;
        }
        break;
    case(EB_ADD):
        switch(result->type){
            case(T_INTEGER):
                result->data.integer = left->data.integer + right->data.integer;
                return result;

            case(T_DOUBLE):
                result->data.dbl = toDouble(left) + toDouble(right);
                return result;
            case(T_STRING):
                result->data.str = str_cat(left->data.str, right->data.str);
                return result;

            default:
            break;
        }
        break;
    case(EB_SUBTRACT):
        switch(result->type){
            case(T_INTEGER):
                result->data.integer = left->data.integer - right->data.integer;
                return result;

            case(T_DOUBLE):
                result->data.dbl = toDouble(left) - toDouble(right);
                return result;

            default:
            break;
        }
        break;
    default:
        break;
    }

    result->undefined = true;

    return result;
}

double toDouble(Value *val){

    if(val->type == T_DOUBLE){
        return val->data.dbl;
    }
    else{
        return (double)(val->data.integer);
    }


}

Value *evalStaticExpression(Expression *e) {
    switch (e->type) {
        case E_FUNCALL:
        case E_REFERENCE:
            MERROR(ERR_SYNTAX, "Cannot use dynamic expressions during static variable initialization!");
        case E_VALUE:
            return e->data.value;
        case E_BINARY:
            return evalBinaryExpression(e->data.binary.op,
                                        evalStaticExpression(e->data.binary.left),
                                        evalStaticExpression(e->data.binary.right));
    }
    return NULL; //Just to pacify the compiler...
}

Value *evalExpression(SymbolTable *symTable, Stack *stack, char *className, Expression *e) {

    if(e == NULL)
        return NULL;

    (void) stack;

    Node *node = NULL;
    Stack *localStack;
    SymbolTable *localSymTable;
    Expression *exp;

    switch (e->type) {
        case E_FUNCALL:

            localStack = createLocalStack(GlobalStack);
            localSymTable = createSymbolTable();

            exp = e->data.funcall.argHead;
            while (exp != NULL) {
                pushParamToStack(localSymTable, localStack, className, exp);
                exp = exp->next;
            }

            evalFunction(localStack,
                         localSymTable,
                         e->data.funcall.name,
                         e->data.funcall.argCount,
                         e->data.funcall.argHead);

            return popFromStack(GlobalStack);

        case E_REFERENCE:
            node = table_lookup_either(symTableGlob, symTable, className, e->data.reference);
            return node->data.value;

        case E_VALUE:
            return e->data.value;

        case E_BINARY:
            return evalBinaryExpression(e->data.binary.op,
                                        evalExpression(symTable, stack, className, e->data.binary.left),
                                        evalExpression(symTable, stack, className, e->data.binary.right));
    }
    return NULL; //Just to pacify the compiler...
}


ValueType evalReturnType( BinaryOperation op, Value *left, Value *right) {

    assert(left != NULL);
    assert(right != NULL);

    ValueType returnType = T_VOID;

    printf("left: %s, right %s\n",showValueType(left->type), showValueType(right->type));

    if((left->type == T_VOID) || (right->type == T_VOID))
        return returnType;

    switch (op) {
        case EB_EQUAL:
        case EB_NOT_EQUAL:
        case EB_LESS:
        case EB_LESS_EQUAL:
        case EB_GREATER:
        case EB_GREATER_EQUAL:

            // if both arguments are equal type but are not string
            if(left->type == right->type && left->type != T_STRING){
                returnType = T_BOOLEAN;
            }
            break;

        case EB_ADD:
            if(left->type == T_STRING && right->type == T_STRING){
                returnType = T_STRING;
                break;
            }
        case EB_MULTIPLY:
        case EB_SUBTRACT:

            if(left->type == right->type){
                returnType = left->type;
            }
            // if is at least one argument double result is also double
            else if((left->type == T_DOUBLE || right->type == T_DOUBLE) && (left->type != T_BOOLEAN && right->type != T_BOOLEAN)){
                returnType = T_DOUBLE;
            }
            else{
                returnType = T_BOOLEAN;
            }
            break;

        case EB_DIVIDE:
           if(left->type == T_INTEGER && T_INTEGER == right->type){
                returnType = T_INTEGER;
            }
            else if(left->type == T_DOUBLE && T_DOUBLE == right->type){
                returnType = T_DOUBLE;
            }
            else
                returnType = T_VOID;
            break;
    }
    printf("return %s\n",showValueType(returnType));

    return returnType;
}

char *str_cat(char *str1, char* str2){

    assert(str1 == NULL);
    assert(str2 == NULL);

    char *result = malloc_c( sizeof(char) * ( strlen(str1) + strlen(str2) ) );
    CHECK_ALLOC(result);

    unsigned x = 0;

    for ( x = 0; x < strlen(str1); x++)
        result[x] = str1[x];
    for (; x < strlen(str2) + strlen(str1); x++)
        result[x] = str2[x - strlen(str1)];

    return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~Function handling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//  Stack Init Del Push Pop

Stack *createLocalStack(Stack *stack){

    Stack *tmp = malloc_c(sizeof(Stack) + 5 * sizeof(Value*));
    CHECK_ALLOC(tmp);

    tmp->prev = stack;

    //  alocate space for 5 params.
    tmp->data[0] = NULL;

    tmp->size = -1;
    tmp->cap = 5;

    return tmp;
}

Stack *deleteLocaleStack(Stack *stack) {
    assert(stack != NULL);

    Stack *tmp = stack->prev;
    free(stack);

    return tmp;
}

int pushToStack(Stack *stack, Value *val){
    assert(stack != NULL);
    assert(val != NULL);

    stack->size++;

    //reallocate whole stack if is no space in stack;
    if(stack->size == stack->cap){
        stack = realloc( stack, sizeof(stack) + stack->cap * 2);
        CHECK_ALLOC(stack->data);
    }

    stack->data[stack->size] = val;

    return 0;
}

Value *popFromStack(Stack *stack){
    assert(stack != NULL);

    if (stack->size == -1) {
        return NULL;
    }
    return stack->data[stack->size--];
}
