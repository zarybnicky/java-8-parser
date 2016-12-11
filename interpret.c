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

bool jirkaDouble = false;

static SymbolTable *symTableGlob = NULL;
static Stack *GlobalStack = NULL;


// cycle helpers.
static bool continueFlag = false;
static bool breakFlag = false;
static bool returnFlag = false;



#define FN(i, ret, fnName, count, arg) do {                             \
        char *name = strdup_("ifj16." #fnName);                         \
        Function *f = createFunction(name, ret, count, arg);            \
        f->builtin = true;                                              \
        table_insert_function(&i->symTable, f);                         \
    } while (0);

Interpret *createInterpret(void) {
    ///Invalid to free twice!?!?
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

// Free symbolTable
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

    free_c(GlobalStack);
    return 0;
}

int interpretFunc(Stack *stack, Node *node) {
    Function *f = node->data.function;

    SymbolTable *localTable = createSymbolTable();
    CHECK_ALLOC(localTable);
    dPrintf("%s", "Creating new local table.\n");

    for (Command *c = f->body.head; c != NULL; c = c->next) {
        char *className=getClassName(f->name);
        evalCommand(localTable, stack, c, className);
        free_c(className);
    }
    if (localTable != NULL)
        freeSymbolTable(localTable);
    return 0;
}

// A core of every cycle is basicaly the same -> why not use some macros?
#define CYCLE_INNER(symTable, stack, funcName, body, end)       \
    do {                                                        \
        for (Command *c = body.head; c != NULL; c = c->next) {  \
            evalCommand(symTable, stack, c, funcName);          \
            if (breakFlag) {                                    \
                breakFlag = false;                              \
                goto end;                                       \
            }                                                   \
            if (continueFlag) {                                 \
                continueFlag = false;                           \
                break;                                          \
            }                                                   \
        }                                                       \
    } while (0);


Value *evalCommand(SymbolTable *symTable, Stack *stack, Command *cmd, char *className){

    Node *node;
    Value *val = NULL;

    // Initialize cycle helpers
    continueFlag = false;
    breakFlag = false;
    returnFlag = false;

    //switch for command type
    dPrintf("CMD TYPE:%s",showCommandType(cmd->type));
    switch(cmd->type){
        case C_DECLARE:
            table_insert_dummy(symTable, cmd->data.declare);
            break;

        case C_DEFINE:
            val = evalExpression(symTable, stack, className, cmd->data.define.expr);

            #ifdef DEBUG
            printf("VALUE BEFORE:\n");
            printValue(val);
            #endif

            Value *newValue = NULL;
            newValue = coerceTo(cmd->data.define.declaration.type, val);

            #ifdef DEBUG
            printf("\nNEWVALUE AFTER:\n");
            printValue(newValue);
            printf("\n");
            #endif

            table_insert(symTable, createValueNode(cmd->data.define.declaration.name, newValue));
            break;

        case C_ASSIGN:
            node = table_lookup_either(symTableGlob, symTable, className, cmd->data.assign.name);
            if(node == NULL)
                MERROR(ERR_INTERNAL, "Interpret: CMD: Assign: Variable not found in local or global symbol table.");
            val = evalExpression(symTable, stack, className, cmd->data.assign.expr);
            // printValue(val);
            if(val == NULL)
                MERROR(ERR_INTERNAL, "Interpret: CMD: Assign: Evaluation of value was not successful.")

            // assign
            node->data.value = coerceTo(node->data.value->type, val);
            break;

        case C_BLOCK:
            evalBlock(symTable, stack, &(cmd->data.block), className);
            break;

        case C_IF:
            if (evalCondition(symTable, stack, className, cmd->data.ifC.cond)) {
                evalBlock(symTable, stack, &(cmd->data.ifC.thenBlock), className);
            } else {
                evalBlock(symTable, stack, &(cmd->data.ifC.elseBlock), className);
            }
            break;

        case C_EXPRESSION:
            val = evalExpression(symTable, stack, className, cmd->data.expr);
            break;

        case C_RETURN:
            if(cmd->data.expr != NULL){
                val = evalExpression(symTable, stack, className, cmd->data.expr);
                pushToStack(GlobalStack, val);
            }
            returnFlag = true;
            break;

        case C_CONTINUE:
            continueFlag = true;
            break;

        case C_BREAK:
            breakFlag = true;
            break;

        case C_FOR:
            if (cmd->data.forC.initial != NULL) {
                val = evalExpression(symTable, stack, className, cmd->data.forC.initial);
                val = coerceTo(cmd->data.forC.var.type, val);
            } else {
                val = createValue(cmd->data.forC.var.type);
                val->undefined = true;
            }
            node = createValueNode(cmd->data.forC.var.name, val);
            table_insert(symTable, node);

            while (evalCondition(symTable, stack, className, cmd->data.forC.cond)){
                CYCLE_INNER(symTable, stack, className, cmd->data.forC.bodyBlock, for_end);

                val = evalCommand(symTable, stack, cmd->data.forC.iter, className);
                node->data.value = coerceTo(cmd->data.forC.var.type, val);
            }
    for_end:
            table_remove(symTable, cmd->data.forC.var.name);
            break;

        case C_WHILE:
            while (evalCondition(symTable, stack, className, cmd->data.whileC.cond)) {
                CYCLE_INNER(symTable, stack, className, cmd->data.whileC.bodyBlock, while_end);
            }
    while_end:
            break;

        case C_DO_WHILE:
            do {
                // #ifdef DEBUG
                // printExpression(cmd->data.doWhileC.cond);
                // #endif
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
    return B(val);
}

int evalBlock(SymbolTable *symTable, Stack *stack, Block *block, char *className){
    Command *current = block->head;

    while (current != NULL) {
        if(continueFlag)
            continueFlag = false;
        evalCommand(symTable, stack, current,className);

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

Value *evalFunction(Stack *localStack, SymbolTable* localSymTable, char *name, int argCount, Expression *argHead, char *className){
    (void) argCount;
    (void) argHead;

    Value *val = NULL;
    #ifdef DEBUG
    printf("evalFunction name: %s\n evalFunction: className %s\n", name,className);
    #endif
    Node *node = table_lookup_either(symTableGlob,NULL,className, name);
    Function *fn = node->data.function;

    // First check for builtins
    if (fn->builtin) {
        builtInFunc(localSymTable, localStack, fn);
        //freeSymbolTable(localTable);
        return popFromStack(localStack);
    }
    // Else push params to local sym table
    Declaration *d = fn->argHead;
    while (localStack->size > -1) {
        val = popFromStack(localStack);
        node=createValueNode(d->name,val);
        table_insert(localSymTable,node);
        d=d->next;
    }
    val = createValue(fn->returnType);
    evalBlock(localSymTable, localStack, &(fn->body), className);

    return val;
}

/**
 * Look for builtin functions
 */
int builtInFunc(SymbolTable *symTable, Stack *stack, Function *fn){
    returnFlag = true;
    jirkaDouble = true;
    char *str = fn->name;

    if(!strcmp(str, "ifj16.print")){
        Value *v = coerceTo(T_STRING, popFromStack(stack));
        // printf("v.data:'%s'\n",v->data.str);
        print(v);
        returnFlag = false;
        return 0;
    }
    else if(!strcmp(str, "ifj16.readInt") ){
        Value *val = createValue(T_INTEGER);
        I(val) = readInt();
        if (I(val) == ERR_RUNTIME_INT_PARSE){
            printSymbolTable(symTable);
            if (symTable != NULL)
                freeSymbolTable(symTable);
            free_c(stack);
            MERROR(ERR_RUNTIME_INT_PARSE, "");
        }
        pushToStack(GlobalStack, val);
        return 0;
    }
    else if(!strcmp(str, "ifj16.readDouble") ){
        Value *val = createValue(T_DOUBLE);
        D(val) = readDouble();
        if (D(val) == ERR_RUNTIME_INT_PARSE){
            freeSymbolTable(symTable);
            free_c(stack);
            MERROR(ERR_RUNTIME_INT_PARSE, "");
        }
        pushToStack(GlobalStack, val);
        return 0;
    }
    else if(!strcmp(str, "ifj16.readString") ){
        Value *val = createValue(T_STRING);
        S(val) = readString();

        pushToStack(GlobalStack, val);
        return 0;
    }
    else if(!strcmp(str, "ifj16.length") ){
        Value *v = coerceTo(T_STRING, popFromStack(stack));

        Value *val = createValue(T_INTEGER);
        I(val) = length(S(v));

        pushToStack(GlobalStack, val);
        return 0;
    }
    else if(!strcmp(str, "ifj16.substr") ){
        Value *n = coerceTo(T_INTEGER, popFromStack(stack));
        Value *i = coerceTo(T_INTEGER, popFromStack(stack));
        Value *s = coerceTo(T_STRING, popFromStack(stack));

        Value *val = createValue(T_STRING);
        S(val) = substr(S(s), I(i), I(n));

        pushToStack(GlobalStack, val);
        return 0;
    }
    else if (!strcmp(str, "ifj16.compare")) {
        Value *s2 = coerceTo(T_STRING, popFromStack(stack));
        Value *s1 = coerceTo(T_STRING, popFromStack(stack));

        Value *val = createValue(T_INTEGER);
        I(val) = compare(S(s1), S(s2));

        pushToStack(GlobalStack, val);
        return 0;
    }
    else if(!strcmp(str, "ifj16.sort") ){
        Value *s = coerceTo(T_STRING, popFromStack(stack));

        Value *val = createValue(T_STRING);
        S(val) = sort(S(s));

        pushToStack(GlobalStack, val);
        return 0;
    }
    else if(!strcmp(str, "ifj16.find") ){
        Value *s2 = coerceTo(T_STRING, popFromStack(stack));
        Value *s1 = coerceTo(T_STRING, popFromStack(stack));

        Value *val = createValue(T_INTEGER);
        I(val) = find(S(s1), S(s2));

        pushToStack(GlobalStack, val);
        return 0;
    }

    return -1;
}

int pushParamToStack(SymbolTable *symTable, Stack *stack, char* funcName, Expression *e) {
    assert(e != NULL);
    pushToStack(stack, evalExpression(symTable, stack, funcName, e));
    return 0;
}

Value *evalUnaryExpression(UnaryOperation op, Value *v) {
    if (v->undefined)
        ERROR(ERR_RUNTIME_UNINITIALIZED);

    #ifdef DEBUG
    printf("val: \n"); printValue(v);
    printf("\n");
    #endif

    Value *r;

    if (v->type == T_BOOLEAN && op == U_NOT) {
        r = createValue(T_BOOLEAN);
        B(r) = !B(v);
        return r;
    }

    if (v->type != T_DOUBLE && v->type != T_INTEGER) {
        r = createValue(T_VOID);
        r->undefined = true;
        return r;
    }

    switch (op) {
    case U_PREINC:
    case U_POSTINC:
    case U_PREDEC:
    case U_POSTDEC:
        return v; //FIXME: increment while evaluating the reference
    case U_NEG:
        break;
    case U_NOT:
        if (v->type == T_INTEGER) {
            r = createValue(T_INTEGER);
            I(r) = -I(v);
            return r;
        } else if (v->type == T_DOUBLE) {
            r = createValue(T_DOUBLE);
            D(r) = -D(v);
            return r;
        }
        break;
    }

    //...the compiler can't do enum switch path analysis
    r = createValue(T_VOID);
    r->undefined = true;
    return r;
}

Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right) {
    if (left->undefined || right->undefined)
        ERROR(ERR_RUNTIME_UNINITIALIZED);

    #ifdef DEBUG
    printf("left: "); printValue(left);
    printf(" right: "); printValue(right);
    printf("\n");
    #endif

    Value *result = createValue(T_BOOLEAN);

    if (left->type == T_VOID || right->type == T_VOID) {
        result->undefined = true;
        return result;
    }

    if (left->type == T_STRING || right->type == T_STRING) {
        if (op != EB_ADD) {
            result->undefined = true;
            return result;
        }
        char *l = ( left->type == T_STRING) ? S( left) : S(coerceTo(T_STRING, left));
        char *r = (right->type == T_STRING) ? S(right) : S(coerceTo(T_STRING, right));
        result->type = T_STRING; // malloc_c
        S(result) = malloc_c(sizeof(char) * (strlen(l) + strlen(r) + 1));
        strcpy(S(result), l);
        strcat(S(result), r);
        return result;
    }

    if (left->type == T_BOOLEAN && right->type == T_BOOLEAN) {
        switch(op){
        case EB_EQUAL:
            B(result) = B(left) == B(right);
            return result;
        case EB_NOT_EQUAL:
            B(result) = B(left) != B(right);
            return result;
        case EB_AND:
            B(result) = B(left) && B(right);
            return result;
        case EB_OR:
            B(result) = B(left) || B(right);
            return result;
        default:
            result->undefined = true;
            return result;
        }
    }

    if (left->type == T_INTEGER && right->type == T_INTEGER) {
        switch(op){
        case EB_EQUAL:
            B(result) = I(left) == I(right);
            return result;
        case EB_NOT_EQUAL:
            B(result) = I(left) != I(right);
            return result;
        case EB_LESS:
            B(result) = I(left) < I(right);
            return result;
        case EB_LESS_EQUAL:
            B(result) = I(left) <= I(right);
            return result;
        case EB_GREATER:
            B(result) = I(left) > I(right);
            return result;
        case EB_GREATER_EQUAL:
            B(result) = I(left) >= I(right);
            return result;
        case EB_ADD:
            result->type = T_INTEGER;
            I(result) = I(left) + I(right);
            return result;
        case EB_SUBTRACT:
            result->type = T_INTEGER;
            I(result) = I(left) - I(right);
            return result;
        case EB_MULTIPLY:
            result->type = T_INTEGER;
            I(result) = I(left) * I(right);
            return result;
        case EB_DIVIDE:
            if (I(right) == 0)
                ERROR(ERR_RUNTIME_DIV_BY_ZERO);
            result->type = T_INTEGER;
            I(result) = I(left) / I(right);
            return result;
        default:
            result->undefined = true;
            return result;
        }
    }

    if (( left->type == T_DOUBLE ||  left->type == T_INTEGER) &&
        (right->type == T_DOUBLE || right->type == T_INTEGER)) {
        switch(op) {
        case EB_EQUAL:
            B(result) = DVAL(left) == DVAL(right);
            return result;
        case EB_NOT_EQUAL:
            B(result) = DVAL(left) != DVAL(right);
            return result;
        case EB_LESS:
            B(result) = DVAL(left) < DVAL(right);
            return result;
        case EB_LESS_EQUAL:
            B(result) = DVAL(left) <= DVAL(right);
            return result;
        case EB_GREATER:
            B(result) = DVAL(left) > DVAL(right);
            return result;
        case EB_GREATER_EQUAL:
            B(result) = DVAL(left) >= DVAL(right);
            return result;
        case EB_ADD:
            result->type = T_DOUBLE;
            D(result) = DVAL(left) + DVAL(right);
            return result;
        case EB_SUBTRACT:
            result->type = T_DOUBLE;
            D(result) = DVAL(left) - DVAL(right);
            return result;
        case EB_MULTIPLY:
            result->type = T_DOUBLE;
            D(result) = DVAL(left) * DVAL(right);
            return result;
        case EB_DIVIDE:
            if (fabs(DVAL(right)) < 10e-7)
                ERROR(ERR_RUNTIME_DIV_BY_ZERO);
            result->type = T_DOUBLE;
            D(result) = DVAL(left) / DVAL(right);
            return result;
        case EB_AND:
        case EB_OR:
            result->undefined = true;
            return result;
        }
    }

    result->undefined = true;
    return result;
}

Value *evalStaticExpression(Expression *e) {
    switch (e->type) {
    case E_FUNCALL:
    case E_REFERENCE:
        MERROR(ERR_SEM_MISC, "Cannot use dynamic expressions during static variable initialization!");
    case E_VALUE:
        return e->data.value;
    case E_UNARY:
        return evalUnaryExpression(e->data.unary.op,
                                   evalStaticExpression(e->data.unary.e));
    case E_BINARY:
        return evalBinaryExpression(e->data.binary.op,
                                    evalStaticExpression(e->data.binary.left),
                                    evalStaticExpression(e->data.binary.right));
    }
    return NULL; //Just to pacify the compiler...
}

Value *evalExpression(SymbolTable *symTable, Stack *stack, char *className, Expression *e) {

    if (e == NULL) {
        MERROR(ERR_INTERNAL, "evalExpression: Expression je null");
        return NULL;
    }

    Node *node = NULL;
    Value *val = NULL;
    Stack *localStack;
    SymbolTable *localSymTable;
    Expression *exp;

    switch (e->type) {
        case E_FUNCALL:
            localStack = createLocalStack(GlobalStack);
            localSymTable = createSymbolTable();

            exp = e->data.funcall.argHead;
            while (exp != NULL) {
                pushToStack(localStack, evalExpression(symTable, localStack, className, exp));
                exp = exp->next;
            }

            if (strchr(e->data.funcall.name, '.') != NULL) {
                className = getClassName(e->data.funcall.name);
            }

            evalFunction(localStack,
                         localSymTable,
                         e->data.funcall.name,
                         e->data.funcall.argCount,
                         e->data.funcall.argHead,
                         className);

            if (returnFlag) {
                val = popFromStack(GlobalStack);
                returnFlag = false;
            }
            free_c(localStack);

            return val;

        case E_REFERENCE:
            #ifdef DEBUG
            printf("reference lookup: %s\n", e->data.reference);
            #endif
            node = table_lookup_either(symTableGlob, symTable, className, e->data.reference);
            return node->data.value;
        case E_VALUE:
            if (e->data.value->undefined) {
                MERROR(ERR_RUNTIME_UNINITIALIZED, "Trying to work with uninitialized value");
            }
            return e->data.value;

        case E_BINARY:
            return evalBinaryExpression(e->data.binary.op,
                                        evalExpression(symTable, stack, className, e->data.binary.left),
                                        evalExpression(symTable, stack, className, e->data.binary.right));
        case E_UNARY:
            // FIXME
            break;
    }
    return NULL; //Just to pacify the compiler...
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~Function handling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void printStack(Stack *stack){
    if(stack == NULL){
        printf("Stack is not initialized\n");
    }

    printf("size: %d cap: %d\n", stack->size, stack->cap);

    printf("|-----------------|\n ");

    for (int i = stack->size; i >= 0; i--) {
        printValue(stack->data[i]);
        printf("\n|-----------------|\n ");
    }
}


Stack *createLocalStack(Stack *stack) {
    //alocate space for 5 params.
    Stack *tmp = malloc_c(sizeof(Stack) + 5 * sizeof(Value*));

    tmp->prev = stack;
    tmp->data[0] = NULL;
    tmp->size = -1;
    tmp->cap = 5;

    return tmp;
}

Stack *deleteLocalStack(Stack *stack) {
    assert(stack != NULL);

    Stack *tmp = stack->prev;
    free_c(stack);

    return tmp;
}

int pushToStack(Stack *stack, Value *val){
    assert(stack != NULL);
    assert(val != NULL);

    stack->size++;

    //reallocate whole stack if there is no space in stack;
    if (stack->size == stack->cap) {
        stack->cap *= 2;
        stack = realloc_c(stack, sizeof(stack) + stack->cap);
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
