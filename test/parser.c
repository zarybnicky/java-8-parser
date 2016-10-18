/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "../ir.h"
#include "../scanner.h"
#include "../parser.h"

Lexer *l;
Interpret *i;
void freeGlobalResources() {
    freeLexer(l);
    freeInterpret(i);
}

int main()
{
    i = createInterpret();
    l = createLexer(NULL, i);
    l->start = l->current =
        chainTokens(createToken(RESERVED, (void *) RES_CLASS, "class"),
                    createToken(ID_SIMPLE, strdup_("Main"), "Main"),
                    createToken(SYMBOL, (void *) SYM_BRACE_OPEN, "{"),

                    createToken(RESERVED, (void *) RES_STATIC, "static"),
                    createToken(RESERVED, (void *) RES_INT, "int"),
                    createToken(ID_SIMPLE, strdup_("run_"), "run_"),
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
    printSymbolTable(&i->symTable);

    return 0;
}
