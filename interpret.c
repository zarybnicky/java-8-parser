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
    Interpret *i = malloc_c(sizeof(Interpret));
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
    free_c(i);
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
            val = coerceTo(cmd->data.define.declaration.type,
                           evalExpression(symTable, stack, className, cmd->data.define.expr, U_ID));
            table_insert(symTable, createValueNode(cmd->data.define.declaration.name, val));
            break;

        case C_ASSIGN:
            node = table_lookup_either(symTableGlob, symTable, className, cmd->data.assign.name);
            if(node == NULL)
                MERROR(ERR_INTERNAL, "Interpret: CMD: Assign: Variable not found in local or global symbol table.");
            val = evalExpression(symTable, stack, className, cmd->data.assign.expr, U_ID);
            if (val == NULL)
                MERROR(ERR_INTERNAL, "Interpret: CMD: Assign: Evaluation of value was not successful.")

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
            val = evalExpression(symTable, stack, className, cmd->data.expr, U_ID);
            break;

        case C_RETURN:
            if(cmd->data.expr != NULL){
                val = evalExpression(symTable, stack, className, cmd->data.expr, U_ID);
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
                val = evalExpression(symTable, stack, className, cmd->data.forC.initial, U_ID);
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
                CYCLE_INNER(symTable, stack, className, cmd->data.doWhileC.bodyBlock, doWhile_end);
            } while (evalCondition(symTable, stack, className, cmd->data.doWhileC.cond));
    doWhile_end:
            break;
    }

    return val;
}

bool evalCondition(SymbolTable *symTable, Stack *stack, char *funcName, Expression *cond) {
    Value *val = evalExpression(symTable, stack, funcName, cond, U_ID);
    if (val == NULL)
        MERROR(ERR_RUNTIME_MISC, "Interpret: CMD: evalCondition: Evaluation was unsuccessful.");
    if (val->type != T_BOOLEAN)
        MERROR(ERR_RUNTIME_MISC, "Interpret: CMD: evalCondition: Condition is not a boolean expression");
    return B(val);
}

int evalBlock(SymbolTable *symTable, Stack *stack, Block *block, char *className){
    Command *current = block->head;

    while (current != NULL) {
        if (continueFlag)
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

void evalFunction(Stack *localStack, SymbolTable* localSymTable, Function *fn, char *className) {
    #ifdef DEBUG
    printf("evalFunction: %s\n", fn->name);
    #endif

    // First check for builtins
    if (fn->builtin) {
        builtInFunc(localStack, fn);
    }
    // Else push params to local sym table
    Declaration *d = fn->argHead;
    while (localStack->size > -1) {
        Value *val = popFromStack(localStack);
        table_insert(localSymTable, createValueNode(d->name, val));
        d = d->next;
    }
    evalBlock(localSymTable, localStack, &(fn->body), className);
}

/**
 * Look for builtin functions
 */
int builtInFunc(Stack *stack, Function *fn){
    returnFlag = true;
    jirkaDouble = true;
    char *str = fn->name;

    if(!strcmp(str, "ifj16.print")){
        Value *v = coerceTo(T_STRING, popFromStack(stack));
        printf("%s", v->data.str);
        returnFlag = false;
        return 0;
    }
    else if(!strcmp(str, "ifj16.readInt") ){
        Value *val = createValue(T_INTEGER);
        I(val) = readInt();
        pushToStack(GlobalStack, val);
        return 0;
    }
    else if(!strcmp(str, "ifj16.readDouble") ){
        Value *val = createValue(T_DOUBLE);
        D(val) = readDouble();
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
        I(val) = strlen(S(v));

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
        int i = strcmp(S(s1), S(s2));
        I(val) =
            i < 0 ? -1 :
            i > 0 ? 1 :
            0;

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
    pushToStack(stack, evalExpression(symTable, stack, funcName, e, U_ID));
    return 0;
}

Value *evalUnaryExpression(UnaryOperation op, Value *v) {
    if (v->undefined) {
        ERROR(ERR_RUNTIME_UNINITIALIZED);
    }

    if (v->type == T_BOOLEAN && op == U_NOT) {
        Value *r = createValue(T_BOOLEAN);
        B(r) = !B(v);
        return r;
    }

    if (v->type != T_DOUBLE && v->type != T_INTEGER) {
        Value *r = createValue(T_VOID);
        r->undefined = true;
        return r;
    }

    switch (op) {
    case U_ID:
    case U_PREINC:
    case U_POSTINC:
    case U_PREDEC:
    case U_POSTDEC:
        return v; //FIXME: increment while evaluating the reference
    case U_NEG:
        if (v->type == T_INTEGER) {
            Value *r = createValue(T_INTEGER);
            I(r) = -I(v);
            return r;
        } else if (v->type == T_DOUBLE) {
            Value *r = createValue(T_DOUBLE);
            D(r) = -D(v);
            return r;
        }
        break;
    case U_NOT:
        if (v->type == T_BOOLEAN) {
            Value *r = createValue(T_BOOLEAN);
            B(r) = !B(v);
            return r;
        }
        break;
    }

    //...the compiler can't do enum switch path analysis
    Value *r = createValue(T_VOID);
    r->undefined = true;
    return r;
}

Value *evalBinaryExpression(BinaryOperation op, Value *left, Value *right) {
    if (left->undefined || right->undefined) {
        ERROR(ERR_RUNTIME_UNINITIALIZED);
    }

    if (left->type == T_VOID || right->type == T_VOID) {
        Value *result = createValue(T_VOID);
        result->undefined = true;
        return result;
    }

    if (left->type == T_STRING || right->type == T_STRING) {
        if (op != EB_ADD) {
            Value *result = createValue(T_VOID);
            result->undefined = true;
            return result;
        }
        char *l = ( left->type == T_STRING) ? S( left) : S(coerceTo(T_STRING, left));
        char *r = (right->type == T_STRING) ? S(right) : S(coerceTo(T_STRING, right));
        Value *result = createValue(T_STRING);
        S(result) = malloc_c(sizeof(char) * (strlen(l) + strlen(r) + 1));
        strcpy(S(result), l);
        strcat(S(result), r);
        return result;
    }

    if (left->type == T_BOOLEAN && right->type == T_BOOLEAN) {
        Value *result = createValue(T_BOOLEAN);
        switch(op) {
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
            return evalBinaryExpression(op, coerceTo(T_STRING, left), coerceTo(T_STRING, right));
        }
    }

    if (left->type == T_INTEGER && right->type == T_INTEGER) {
        Value *result = createValue(T_BOOLEAN);
        switch(op) {
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
            return evalBinaryExpression(op, coerceTo(T_STRING, left), coerceTo(T_STRING, right));
        }
    }

    if (( left->type == T_DOUBLE ||  left->type == T_INTEGER) &&
        (right->type == T_DOUBLE || right->type == T_INTEGER)) {
        Value *result = createValue(T_BOOLEAN);
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
            return evalBinaryExpression(op, coerceTo(T_STRING, left), coerceTo(T_STRING, right));
        }
    }

    Value *result = createValue(T_VOID);
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

Value *evalExpression(SymbolTable *symTable, Stack *stack, char *className, Expression *e, UnaryOperation op) {
    if (e == NULL) {
        MERROR(ERR_INTERNAL, "evalExpression: Expression je null");
    }

    Value *val = NULL;
    Stack *localStack;
    SymbolTable *localSymTable;
    Expression *exp;
    static Value one = { .type = T_INTEGER, .data = { .integer = 1 } };

    switch (e->type) {
    case E_FUNCALL:
        if (op > U_ID && op < U_NOT) {
            MERROR(ERR_INTERNAL, "Trying to pre/postinc/dec a non-reference");
        }

        localStack = createLocalStack(GlobalStack);
        localSymTable = createSymbolTable();

        exp = e->data.funcall.argHead;
        while (exp != NULL) {
            pushToStack(localStack, evalExpression(symTable, localStack, className, exp, U_ID));
            exp = exp->next;
        }

        if (strchr(e->data.funcall.name, '.') != NULL) {
            className = getClassName(e->data.funcall.name);
        }

        Node *node = table_lookup_either(symTableGlob, NULL, className, e->data.funcall.name);
        Function *f = node->data.function;

        evalFunction(localStack, localSymTable, f, className);
        free_c(localStack);

        if (returnFlag) {
            returnFlag = false;
            return coerceTo(f->returnType, popFromStack(GlobalStack));
        }
        return NULL;

    case E_REFERENCE:
        node = table_lookup_either(symTableGlob, symTable, className, e->data.reference);
        val = node->data.value;
        switch (op) {
        case U_ID:
        case U_NOT:
        case U_NEG:
            break;
        case U_PREINC:
            node->data.value = evalBinaryExpression(EB_ADD, node->data.value, &one);
            break;
        case U_PREDEC:
            node->data.value = evalBinaryExpression(EB_SUBTRACT, node->data.value, &one);
            break;
        case U_POSTINC:
            val = copyValue(val);
            node->data.value = evalBinaryExpression(EB_ADD, node->data.value, &one);
            break;
        case U_POSTDEC:
            val = copyValue(val);
            node->data.value = evalBinaryExpression(EB_SUBTRACT, node->data.value, &one);
            break;
        }
        return val;

    case E_VALUE:
        if (op > U_ID && op < U_NOT) {
            MERROR(ERR_INTERNAL, "Trying to pre/postinc/dec a non-reference");
        }
        if (e->data.value->undefined) {
            MERROR(ERR_RUNTIME_UNINITIALIZED, "Trying to work with uninitialized value");
        }
        return e->data.value;

    case E_BINARY:
        if (op > U_ID && op < U_NOT) {
            MERROR(ERR_INTERNAL, "Trying to pre/postinc/dec a non-reference");
        }
        return evalBinaryExpression(e->data.binary.op,
                                    evalExpression(symTable, stack, className, e->data.binary.left, U_ID),
                                    evalExpression(symTable, stack, className, e->data.binary.right, U_ID));

    case E_UNARY:
        if (op > U_ID && op < U_NOT) {
            MERROR(ERR_INTERNAL, "Trying to pre/postinc/dec a non-reference");
        }
        return evalUnaryExpression(e->data.unary.op,
                                   evalExpression(symTable, stack, className, e->data.unary.e, e->data.unary.op));
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
    //allocate space for 5 params.
    Stack *tmp = malloc_c(sizeof(Stack) + 5 * sizeof(Value *));

    tmp->prev = stack;
    tmp->data[0] = NULL;
    tmp->size = -1;
    tmp->cap = 5;

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
