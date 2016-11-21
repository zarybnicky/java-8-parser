#include "../ir.h"
#include "../ial.h"
#include "../scanner.h"
#include "../parser.h"

Lexer *l;
Interpret *i;
void freeGlobalResources() {
    freeLexer(l);
    freeInterpret(i);
}

void pre_order(Node *node){
  if (node != NULL){
    printNode(node);
    pre_order(node->left);
    pre_order(node->right);
  }
}

#define INSERT(tbl, n) do {                                         \
    Declaration a = { .type = T_VOID, .name = n, .next = NULL };    \
    table_insert_dummy(tbl, a);                                     \
    } while (0)

int main() {

    printf("Creating the table...\n");
    SymbolTable *table = createSymbolTable();
    INSERT(table, "a");
    INSERT(table, "b");
    INSERT(table, "c");
    INSERT(table, "d");
    INSERT(table, "e");
    INSERT(table, "f");
    INSERT(table, "g");
    printSymbolTable(table);

    Node *n;
    printf("Looking up a valid node...\n");
    n = table_lookup(table, "c");
    printNode(n);

    printf("Looking up a nonexistent node...\n");
    n = table_lookup(table, "abc");
    printNode(n);

    printf("Removing a node...\n");
    n = table_remove(table, "d");
    freeNode(n);
    printSymbolTable(table);

    printf("Iterating through the table...\n");
    table_iterate(table->root, printNode);

    freeSymbolTable(table);
    return 0;
}
