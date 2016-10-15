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
    Value *v = malloc(sizeof(Value));
    CHECK_ALLOC(v);
    v->type = type;
    return v;
}
void freeValue(Value *v) {
    if (v != NULL) {
        if (v->type == T_STRING) {
            free(v->data.str);
        }
        free(v);
    }
}
void printValue(Value *v) {
    if (v == NULL) {
        printf("Value(NULL)");
        return;
    }
    printf("Value(");
    switch (v->type) {
    case T_STRING:  printf("\"%s\"", v->data.str); break;
    case T_INTEGER: printf("%d", v->data.integer); break;
    case T_DOUBLE:  printf("%g", v->data.dbl);     break;
    case T_BOOLEAN: printf("%d", v->data.boolean); break;
    case T_VOID:    break;
    }
    printf(")");
}

/* EXPRESSION */
Expression *createExpression(ExpressionType type) {
    Expression *e = malloc(sizeof(Expression));
    CHECK_ALLOC(e);
    e->type = type;
    e->next = NULL;
    return e;
}
void freeExpression(Expression *e) {
    Expression *f;
    while (e != NULL) {
        f = e->next;
        switch (e->type) {
        case E_FUNCALL:
            free(e->data.funcall.name);
            freeExpression(e->data.funcall.argHead);
            break;
        case E_VALUE:
            freeValue(e->data.value);
            break;
        case E_REFERENCE:
            free(e->data.reference);
            break;
        case E_BINARY:
            freeExpression(e->data.binary.left);
            freeExpression(e->data.binary.right);
            break;
        }
        free(e);
        e = f;
    }
}
void printExpression(Expression *e) {
    if (e == NULL) {
        printf("Expression(NULL)");
        return;
    }
    do {
        printf("Expression("), printExpressionType(e->type), printf(",");
        switch (e->type) {
        case E_FUNCALL:
            printf("%s, %d:", e->data.funcall.name, e->data.funcall.argCount);
            printExpression(e->data.funcall.argHead);
            break;
        case E_VALUE:
            printValue(e->data.value);
            break;
        case E_REFERENCE:
            printValue(e->data.value);
            break;
        case E_BINARY:
            printBinaryOperation(e->data.binary.op);
            printExpression(e->data.binary.left);
            printExpression(e->data.binary.right);
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
    Declaration *d = malloc(sizeof(Declaration));
    CHECK_ALLOC(d);
    d->type = type;
    d->name = name;
    d->next = NULL;
    return d;
}
void freeDeclaration(Declaration *d) {
    Declaration *e;
    while (d != NULL) {
        e = d->next;
        free(d->name);
        free(d);
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
        printValueType(d->type);
        printf("%s", d->name);
        if (d->next != NULL) {
            printf(", ");
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
    printf("Block {\n");
    printCommand(b->head);
    printf("} Block");
}

/* COMMAND */
Command *createCommand(CommandType type) {
    Command *c = malloc(sizeof(Command));
    CHECK_ALLOC(c);
    c->type = type;
    c->next = NULL;
    return c;
}
void freeCommand(Command *c) {
    if (c == NULL) {
        return;
    }
    Command *d;
    while (c != NULL) {
        d = c->next;
        switch (c->type) {
        case C_DECLARE:
            free(c->data.declare.name);
            break;
        case C_DEFINE:
            free(c->data.define.declaration.name);
            freeExpression(c->data.define.expr);
            break;
        case C_ASSIGN:
            free(c->data.assign.name);
        freeExpression(c->data.assign.expr);
        break;
        case C_BLOCK:
            freeCommand(c->data.block.head);
            break;
        case C_IF:
            freeExpression(c->data.ifC.cond);
            freeBlock(c->data.ifC.thenBlock);
            freeBlock(c->data.ifC.elseBlock);
        case C_WHILE:
            freeExpression(c->data.whileC.cond);
            freeBlock(c->data.whileC.bodyBlock);
            break;
        case C_EXPRESSION:
        case C_RETURN:
            freeExpression(c->data.expr);
            break;
        }
        free(c);
        c = d;
    }
}
void printCommand(Command *c) {
    if (c == NULL) {
        printf("Command(NULL)\n");
        return;
    }
    while (c != NULL) {
        printf("Command("), printCommandType(c->type), printf(", ");
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
        case C_EXPRESSION:
        case C_RETURN:
            printExpression(c->data.expr);
            break;
        }
        printf(")\n");
        c = c->next;
    }
}

/* FUNCTION */
Function *createFunction(char *name, int argCount, Declaration *argHead) {
    Function *f = malloc(sizeof(Function));
    CHECK_ALLOC(f);
    f->name = name;
    f->argCount = argCount;
    f->argHead = argHead;
    return f;
}
void freeFunction(Function *f) {
    if (f != NULL) {
        free(f->name);
        freeDeclaration(f->argHead);
        freeCommand(f->body.head);
        free(f);
    }
}
void printFunction(Function *f) {
    if (f == NULL) {
        printf("Function(NULL)\n");
        return;
    }
    printf("Function(%s, %d:", f->name, f->argCount);
    printDeclaration(f->argHead);
    putchar('\n');
    printBlock(&f->body);
    printf("\n)\n");
}

/* NODE */
Node *createFunctionNode(char *symbol, Function *f) {
    Node *n = malloc(sizeof(Node));
    CHECK_ALLOC(n);
    n->symbol = symbol;
    n->type = N_FUNCTION;
    n->data.function = f;
    n->left = NULL;
    n->right = NULL;
    return n;
}
Node *createValueNode(char *symbol, Value *v) {
    Node *n = malloc(sizeof(Node));
    CHECK_ALLOC(n);
    n->symbol = symbol;
    n->type = N_VALUE;
    n->data.value = v;
    n->left = NULL;
    n->right = NULL;
    return n;
}
void freeNode(Node *n) {
    if (n != NULL) {
        if (n->type == N_VALUE)
            free(n->data.value);
        else
            free(n->data.function);
        freeNode(n->left);
        freeNode(n->right);
        free(n);
    }
}
void printNode(Node *n)  {
    if (n == NULL) {
        printf("Node(NULL)");
    }
    printf("Node(");
    printf("%s, ", n->symbol);
    printNodeType(n->type);
    printf(", ");
    printNode(n->left);
    printf(", ");
    printNode(n->right);
    printf(")\n");
}

/* SYMBOL TABLE */
SymbolTable *createSymbolTable()  {
    SymbolTable *t = malloc(sizeof(SymbolTable));
    CHECK_ALLOC(t);
    t->root = NULL;
    return t;
}
void freeSymbolTable(SymbolTable *t)  {
    if (t != NULL) {
        freeNode(t->root);
        free(t);
    }
}
void printSymbolTable(SymbolTable *t) {
    if (t != NULL) {
        printf("SymbolTable(");
        printNode(t->root);
        printf(")\n");
    }
}

/* ENUMS */
void printValueType(ValueType x) {
    switch (x) {
    case T_STRING:  printf("String"); break;
    case T_INTEGER: printf("Integer"); break;
    case T_DOUBLE:  printf("Double"); break;
    case T_BOOLEAN: printf("Boolean"); break;
    case T_VOID:    printf("Void"); break;
    }
}
void printNodeType(NodeType x) {
    switch (x) {
    case N_VALUE:    printf("Value"); break;
    case N_FUNCTION: printf("Function"); break;
    }
}
void printExpressionType(ExpressionType x) {
    switch (x) {
    case E_FUNCALL:   printf("Funcall"); break;
    case E_VALUE:     printf("Value"); break;
    case E_REFERENCE: printf("Reference"); break;
    case E_BINARY:    printf("Binary"); break;
    }
}
void printBinaryOperation(BinaryOperation x) {
    switch (x) {
    case EB_EQUAL:         printf("Equal"); break;
    case EB_NOT_EQUAL:     printf("Not equal"); break;
    case EB_LESS:          printf("Less"); break;
    case EB_LESS_EQUAL:    printf("Less or equal"); break;
    case EB_GREATER:       printf("Greater"); break;
    case EB_GREATER_EQUAL: printf("Greater or equal"); break;
    case EB_MULTIPLY:      printf("Multiply"); break;
    case EB_DIVIDE:        printf("Divide"); break;
    case EB_ADD:           printf("Add"); break;
    case EB_SUBTRACT:      printf("Subtract"); break;
    }
}
void printCommandType(CommandType x) {
    switch (x) {
    case C_DECLARE:    printf("Declare"); break;
    case C_DEFINE:     printf("Define"); break;
    case C_ASSIGN:     printf("Assign"); break;
    case C_BLOCK:      printf("Block"); break;
    case C_IF:         printf("If"); break;
    case C_WHILE:      printf("While"); break;
    case C_EXPRESSION: printf("Expression"); break;
    case C_RETURN:     printf("Return"); break;
    }
}
