#include "../ir.h"
#include "../scanner.h"
#include "../parser.h"

SymbolTable fnTable = { .root = NULL };
SymbolTable varTable = { .root = NULL };

void freeGlobalResources() {
    freeNode(fnTable.root);
    freeNode(varTable.root);
}

int main()
{
    Lexer *l = createLexer(NULL);
    l->start = l->current =
        chainTokens(createToken(RESERVED, (void *) RES_CLASS, "class"),
                    createToken(ID_SIMPLE, strdup_("Main"), "Main"),
                    createToken(SYMBOL, (void *) SYM_BRACE_OPEN, "{"),

                    createToken(RESERVED, (void *) RES_STATIC, "static"),
                    createToken(RESERVED, (void *) RES_INT, "int"),
                    createToken(ID_SIMPLE, strdup_("run"), "run"),
                    createToken(SYMBOL, (void *) SYM_SEMI, ";"),

                    createToken(RESERVED, (void *) RES_STATIC, "static"),
                    createToken(RESERVED, (void *) RES_VOID, "void"),
                    createToken(ID_SIMPLE, strdup_("run"), "run"),
                    createToken(SYMBOL, (void *) SYM_PAREN_OPEN, "("),
                    createToken(RESERVED, (void *) RES_INT, "int"),
                    createToken(ID_SIMPLE, strdup_("arg1"), "arg1"),
                    createToken(SYMBOL, (void *) SYM_COMMA, ","),
                    createToken(RESERVED, (void *) RES_STRING, "String"),
                    createToken(ID_SIMPLE, strdup_("arg2"), "arg2"),
                    createToken(SYMBOL, (void *) SYM_PAREN_CLOSE, ")"),
                    createToken(SYMBOL, (void *) SYM_BRACE_OPEN, "{"),

                    createToken(ID_COMPOUND, strdup_("ifj16.print"), "ifj16.print"),
                    createToken(SYMBOL, (void *) SYM_ASSIGN, "="),
                    createToken(LIT_INTEGER, (void *) 1, "1"),
                    createToken(SYMBOL, (void *) SYM_PLUS, "+"),
                    createToken(LIT_INTEGER, (void *) 2, "2"),
                    createToken(SYMBOL, (void *) SYM_SEMI, ";"),

                    createToken(SYMBOL, (void *) SYM_BRACE_CLOSE, "}"),
                    createToken(SYMBOL, (void *) SYM_BRACE_CLOSE, "}"),
                    NULL);
    while (hasMore(l)) {
        parseClass(l);
    }

    return 0;
}
