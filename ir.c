/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "ir.h"

/* VALUE */
Value *createValue(ValueType type) {
    Value *v = malloc_c(sizeof(Value));
    v->type = type;
    v->undefined = false;
    return v;
}
void freeValue(Value *v) {
    if (v != NULL) {
        free_c(v);
    }
}
void printValue(Value *v) {
    if (v == NULL) {
        printf("Value(NULL)");
        return;
    }
    if (v->undefined) {
        printf("Value(undefined)");
        return;
    }
    printf("Value(");
    switch (v->type) {
    case T_STRING:  printf("String:\"%s\"", v->data.str); break;
    case T_INTEGER: printf("int:%d", v->data.integer); break;
    case T_DOUBLE:  printf("double:%g", v->data.dbl);     break;
    case T_BOOLEAN: printf("bool:%d", v->data.boolean); break;
    case T_VOID:    printf("void"); break;
    }
    printf(")");
}

/* EXPRESSION */
Expression *createExpression(ExpressionType type) {
    Expression *e = malloc_c(sizeof(Expression));
    e->type = type;
    e->next = NULL;
    return e;
}
void freeExpression(Expression *e) {
    while (e != NULL) {
        Expression *f = e->next;
        switch (e->type) {
        case E_FUNCALL:
            free_c(e->data.funcall.name);
            freeExpression(e->data.funcall.argHead);
            break;
        case E_VALUE:
            freeValue(e->data.value);
            break;
        case E_REFERENCE:
            free_c(e->data.reference);
            break;
        case E_BINARY:
            freeExpression(e->data.binary.left);
            freeExpression(e->data.binary.right);
            break;
        case E_UNARY:
            freeExpression(e->data.unary.e);
            break;
        }
        free_c(e);
        e = f;
    }
}
void printExpression(Expression *e) {
    if (e == NULL) {
        printf("Expression(NULL)");
        return;
    }
    do {
        printf("Expression(%s, ", showExpressionType(e->type));
        switch (e->type) {
        case E_FUNCALL:
            printf("%s, %d:", e->data.funcall.name, e->data.funcall.argCount);
            printExpression(e->data.funcall.argHead);
            break;
        case E_VALUE:
            printValue(e->data.value);
            break;
        case E_REFERENCE:
            printf("%s", e->data.reference);
            break;
        case E_BINARY:
            printf("%s, ", showBinaryOperation(e->data.binary.op));
            printExpression(e->data.binary.left);
            printf(", ");
            printExpression(e->data.binary.right);
            break;
        case E_UNARY:
            printf("%s, ", showUnaryOperation(e->data.unary.op));
            printExpression(e->data.unary.e);
            break;
        }
        printf(")");
        if (e->next != NULL) {
            printf(", ");
        }
        e = e->next;
    } while (e != NULL);
}

/* DECLARATION */
Declaration *createDeclaration(ValueType type, char *name) {
    Declaration *d = malloc_c(sizeof(Declaration));
    d->type = type;
    d->name = name;
    d->next = NULL;
    return d;
}
void freeDeclaration(Declaration *d) {
    while (d != NULL) {
        Declaration *e = d->next;
        free_c(d->name);
        free_c(d);
        d = e;
    }
}
void printDeclaration(Declaration *d) {
    if (d == NULL) {
        printf("Declaration(NULL)");
        return;
    }
    printf("Declaration(");
    do {
        printf("%s %s", showValueType(d->type), d->name);
        if (d->next != NULL) {
            printf(" ---> ");
        }
        d = d->next;
    } while (d != NULL);
    printf(")");
}

void appendToBlock(Block *b, Command *c) {
    c->next = NULL;
    if (b->head == NULL) {
        b->head = b->tail = c;
    } else {
        b->tail->next = c;
        b->tail = c;
    }
}
void freeBlock(Block b) {
    freeCommand(b.head);
}
void printBlock(Block *b) {
    printf("<block>\n");
    printCommand(b->head);
    printf("</block>");
}

/* COMMAND */
#define CREATE_COMMAND(c, type_)                \
    Command *c = malloc_c(sizeof(Command));     \
    c->type = type_;                            \
    c->next = NULL;                             \

Command *createCommandDeclare(Declaration d) {
    CREATE_COMMAND(c, C_DECLARE);
    c->data.declare = d;
    return c;
}
Command *createCommandDefine(Declaration d, Expression *e) {
    CREATE_COMMAND(c, C_DEFINE);
    c->data.define.declaration = d;
    c->data.define.expr = e;
    return c;
}
Command *createCommandAssign(char *n, Expression *e) {
    CREATE_COMMAND(c, C_ASSIGN);
    c->data.assign.name = n;
    c->data.assign.expr = e;
    return c;
}
Command *createCommandBlock() {
    CREATE_COMMAND(c, C_BLOCK);
    c->data.block.head = c->data.block.tail = NULL;
    return c;
}
Command *createCommandIf(Expression *e) {
    CREATE_COMMAND(c, C_IF);
    c->data.ifC.cond = e;
    c->data.ifC.thenBlock.head = c->data.ifC.thenBlock.tail = NULL;
    c->data.ifC.elseBlock.head = c->data.ifC.elseBlock.tail = NULL;
    return c;
}
Command *createCommandWhile(Expression *e) {
    CREATE_COMMAND(c, C_WHILE);
    c->data.whileC.cond = e;
    c->data.whileC.bodyBlock.head = c->data.whileC.bodyBlock.tail = NULL;
    return c;
}
Command *createCommandDoWhile() {
    CREATE_COMMAND(c, C_DO_WHILE);
    c->data.doWhileC.bodyBlock.head = c->data.doWhileC.bodyBlock.tail = NULL;
    return c;
}
Command *createCommandFor(Declaration d, Expression *init, Expression *cond, Command *iter) {
    CREATE_COMMAND(c, C_FOR);
    c->data.forC.var = d;
    c->data.forC.initial = init;
    c->data.forC.cond = cond;
    c->data.forC.iter = iter;
    c->data.forC.bodyBlock.head = c->data.forC.bodyBlock.tail = NULL;
    return c;
}
Command *createCommandExpression(Expression *e) {
    CREATE_COMMAND(c, C_EXPRESSION);
    c->data.expr = e;
    return c;
}
Command *createCommandReturn(Expression *e) {
    CREATE_COMMAND(c, C_RETURN);
    c->data.expr = e;
    return c;
}
Command *createCommandContinue() {
    CREATE_COMMAND(c, C_CONTINUE);
    return c;
}
Command *createCommandBreak() {
    CREATE_COMMAND(c, C_BREAK);
    return c;
}

void freeCommand(Command *c) {
    while (c != NULL) {
        Command *d = c->next;
        switch (c->type) {
        case C_DECLARE:
            free_c(c->data.declare.name);
            break;
        case C_DEFINE:
            free_c(c->data.define.declaration.name);
            freeExpression(c->data.define.expr);
            break;
        case C_ASSIGN:
            free_c(c->data.assign.name);
            freeExpression(c->data.assign.expr);
            break;
        case C_BLOCK:
            freeCommand(c->data.block.head);
            break;
        case C_IF:
            freeExpression(c->data.ifC.cond);
            freeBlock(c->data.ifC.thenBlock);
            freeBlock(c->data.ifC.elseBlock);
            break;
        case C_WHILE:
            freeExpression(c->data.whileC.cond);
            freeBlock(c->data.whileC.bodyBlock);
            break;
        case C_DO_WHILE:
            freeExpression(c->data.doWhileC.cond);
            freeBlock(c->data.doWhileC.bodyBlock);
            break;
        case C_FOR:
            free_c(c->data.forC.var.name);
            freeExpression(c->data.forC.initial);
            freeExpression(c->data.forC.cond);
            freeCommand(c->data.forC.iter);
            freeBlock(c->data.forC.bodyBlock);
            break;
        case C_EXPRESSION:
        case C_RETURN:
            freeExpression(c->data.expr);
            break;
        case C_CONTINUE:
        case C_BREAK:
            break;
        }
        free_c(c);
        c = d;
    }
}
void printCommand(Command *c) {
    if (c == NULL) {
        printf("Command(NULL)\n");
        return;
    }
    while (c != NULL) {
        printf("Command(%s, ", showCommandType(c->type));
        switch (c->type) {
        case C_DECLARE:
            printDeclaration(&c->data.declare);
            break;
        case C_DEFINE:
            printDeclaration(&c->data.define.declaration), printf(", ");
            printExpression(c->data.define.expr);
            break;
        case C_ASSIGN:
            printf("%s, ", c->data.assign.name);
            printExpression(c->data.assign.expr);
            break;
        case C_BLOCK:
            printBlock(&c->data.block);
            break;
        case C_IF:
            printExpression(c->data.ifC.cond), printf(", ");
            printBlock(&c->data.ifC.thenBlock), printf(", ");
            printBlock(&c->data.ifC.elseBlock);
            break;
        case C_WHILE:
            printExpression(c->data.whileC.cond), printf(", ");
            printBlock(&c->data.whileC.bodyBlock);
            break;
        case C_DO_WHILE:
            printExpression(c->data.doWhileC.cond), printf(", ");
            printBlock(&c->data.doWhileC.bodyBlock);
            break;
        case C_FOR:
            printDeclaration(&c->data.forC.var), printf(", ");
            printExpression(c->data.forC.initial), printf(", ");
            printExpression(c->data.forC.cond), printf(", ");
            printCommand(c->data.forC.iter), printf(", ");
            printBlock(&c->data.whileC.bodyBlock);
            break;
        case C_EXPRESSION:
        case C_RETURN:
            printExpression(c->data.expr);
            break;
        case C_CONTINUE:
        case C_BREAK:
            break;
        }
        printf(")\n");
        c = c->next;
    }
}

/* FUNCTION */
Function *createFunction(char *name, ValueType type, int argCount, Declaration *argHead) {
    Function *f = malloc_c(sizeof(Function));
    f->name = name;
    f->returnType = type;
    f->argCount = argCount;
    f->argHead = argHead;
    f->body.head = f->body.tail = NULL;
    f->builtin = false;
    return f;
}
void freeFunction(Function *f) {
    if (f != NULL) {
        free_c(f->name);
        freeDeclaration(f->argHead);
        freeCommand(f->body.head);
        free_c(f);
    }
}
void printFunction(Function *f) {
    if (f == NULL) {
        printf("Function(NULL)\n");
        return;
    }
    printf("Function(%s, %s, ", f->name, showValueType(f->returnType));
    printf("%d:", f->argCount);
    printDeclaration(f->argHead);
    printf(", ");
    printBlock(&f->body);
    printf(")\n");
}
void traverseCommandsC(Function *f, Command *c, void (*fn)(Function *, Command *), void (*onForExit)(Command *)) {
    while (c != NULL) {
        fn(f, c);
        switch (c->type) {
        case C_DECLARE:
        case C_DEFINE:
        case C_ASSIGN:
        case C_EXPRESSION:
        case C_RETURN:
        case C_CONTINUE:
        case C_BREAK:
            break;
        case C_BLOCK:
            traverseCommandsC(f, c->data.block.head, fn, onForExit);
            break;
        case C_IF:
            traverseCommandsC(f, c->data.ifC.thenBlock.head, fn, onForExit);
            traverseCommandsC(f, c->data.ifC.elseBlock.head, fn, onForExit);
            break;
        case C_WHILE:
            traverseCommandsC(f, c->data.whileC.bodyBlock.head, fn, onForExit);
            break;
        case C_DO_WHILE:
            traverseCommandsC(f, c->data.doWhileC.bodyBlock.head, fn, onForExit);
            break;
        case C_FOR:
            traverseCommandsC(f, c->data.forC.iter, fn, onForExit);
            traverseCommandsC(f, c->data.forC.bodyBlock.head, fn, onForExit);
            if (onForExit != NULL)
                onForExit(c);
            break;
        }
        c = c->next;
    }
}
void traverseCommands(Function *f, void (*fn)(Function *, Command *), void (*onForExit)(Command *)) {
    if (f->body.head != NULL) {
        traverseCommandsC(f, f->body.head, fn, onForExit);
    }
}

/* ENUMS */
char *showValueType(ValueType x) {
    switch (x) {
    case T_STRING:  return "String";
    case T_INTEGER: return "Integer";
    case T_DOUBLE:  return "Double";
    case T_BOOLEAN: return "Boolean";
    case T_VOID:    return "Void";
    }
    return "Unknown ValueType"; //Just to pacify the compiler...
}
char *showExpressionType(ExpressionType x) {
    switch (x) {
    case E_FUNCALL:   return "Funcall";
    case E_VALUE:     return "Value";
    case E_REFERENCE: return "Reference";
    case E_BINARY:    return "Binary";
    case E_UNARY:     return "Unary";
    }
    return "Unknown ExpressionType"; //Just to pacify the compiler...
}
char *showUnaryOperation(UnaryOperation x) {
    switch (x) {
    case U_ID:      return "Id";
    case U_NEG:     return "Negate";
    case U_NOT:     return "Not";
    case U_PREINC:  return "PreIncrement";
    case U_POSTINC: return "PostIncrement";
    case U_PREDEC:  return "PreDecrement";
    case U_POSTDEC: return "PostDecrement";
    }
    return "Unknown UnaryOperation"; //Just to pacify the compiler...
}
char *showBinaryOperation(BinaryOperation x) {
    switch (x) {
    case EB_EQUAL:         return "Equal";
    case EB_NOT_EQUAL:     return "Not equal";
    case EB_LESS:          return "Less";
    case EB_LESS_EQUAL:    return "Less or equal";
    case EB_GREATER:       return "Greater";
    case EB_GREATER_EQUAL: return "Greater or equal";
    case EB_MULTIPLY:      return "Multiply";
    case EB_DIVIDE:        return "Divide";
    case EB_ADD:           return "Add";
    case EB_SUBTRACT:      return "Subtract";
    case EB_AND:           return "And";
    case EB_OR:            return "Or";
    }
    return "Unknown BinaryOperation"; //Just to pacify the compiler...
}
char *showCommandType(CommandType x) {
    switch (x) {
    case C_DECLARE:    return "Declare";
    case C_DEFINE:     return "Define";
    case C_ASSIGN:     return "Assign";
    case C_BLOCK:      return "Block";
    case C_IF:         return "If";
    case C_WHILE:      return "While";
    case C_DO_WHILE:   return "DoWhile";
    case C_FOR:        return "For";
    case C_EXPRESSION: return "Expression";
    case C_CONTINUE:   return "Continue";
    case C_BREAK:      return "Break";
    case C_RETURN:     return "Return";
    }
    return "Unknown CommandType"; //Just to pacify the compiler...
}

/* Takes ClassName of function(fn->name), everything before character '.' */
char *getClassName(char *fn){
    char *dot = strchr(fn, '.');
    if (dot == NULL) {
        FERROR(ERR_INTERNAL, "Unqualified function reference found: %s", fn);
    }

    *dot = '\0';
    char *class = strdup_(fn);
    *dot = '.';
    return class;
}

/* Not in POSIX... Makes a copy of string s to char *dup */
char *strdup_(const char *s) {
    if (s == NULL)
        return NULL;
    char *dup = malloc_c(strlen(s) + 1);
    strcpy(dup, s);
    return dup;
}

/* Makes a copy of Value val to new retValue */
Value *copyValue (Value *val){
    if (val == NULL)
        return NULL;
    Value *retValue = createValue(val->type);
    switch(val->type){
    case T_BOOLEAN:
        retValue->data.boolean = B(val);
        return retValue;
    case T_INTEGER:
        retValue->data.integer = I(val);
        return retValue;
    case T_DOUBLE:
        retValue->data.dbl = D(val);
        return retValue;
    default:
        retValue->data.str = S(val);
        return retValue;
    }
}

double hexToDbl(char *str){
    int sLen = strlen(str);
    int count = 0;
    double significant1 = 0;
    double significant2 = 0;
    long int exp = 0;
    int p = 0;
    int dot = 0;
    char *check = NULL;
    char *stmp = malloc_c(sLen*sizeof(char));

    for (int i = 0; i < sLen; i++){
        stmp[i] = '\0';

        if (str[i] != '_'){
            stmp[count] = str[i];
            count++;
        }

        if (str[i] == 'p'){
            if (p != 0){
                MERROR(ERR_INTERNAL, "Too many 'p' in expression.");
            }
            p = i;
        }

        if (str[i] == '.'){
            if (dot != 0){
                MERROR(ERR_INTERNAL, "Too many '.' in expression.");
            }
            dot = i;
        }
    }

    if (dot == p+1 || dot == p-1){
        MERROR(ERR_INTERNAL, "Missing numeric literal.");
    }

    significant1 = (double)strtol(stmp, &check, 0);

    if (check[0] != '\0' && check[0] != '.' && check[0] != 'p'){
        MERROR(ERR_INTERNAL, "Unexpected char in hexadecimal number.");
    }

    if (check[0] != '\0' && check[0] == '.'){
        check++;
        significant2 = (double)strtol(check, &check, 16);

        if (check[0] != '\0' && check[0] != 'p'){
            MERROR(ERR_INTERNAL, "Unexpected char in hexadecimal number.");
        }

        while(significant2 > 1){
            significant2 = significant2 * 0.1;
        }

        significant1 += significant2;
    }

    if (check[0] != '\0' && check[0] == 'p'){
        check++;
        exp = strtol(check, &check, 16);

        if (check[0] != '\0'){
            MERROR(ERR_INTERNAL, "Unexpected char in hexadecimal number.");
        }

        if (exp > 0){
            for (long int i = 0; i < exp; i++){
                significant1 = significant1 * 10;
            }
        } else {
            for (long int i = 0; i < abs(exp); i++){
                significant1 = significant1 * 0.1;
            }
        }
    }
    return significant1;
}
