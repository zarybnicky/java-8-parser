#include "../ir.h"
#include "../ial.h"

int main(){
  char name[10] ="abc";
  Value *new_val = createValue (1);
  new_val->data.integer = 10;
  printValue (new_val);
  printf("\n");

  Node *new_node = NULL;
  new_node = createValueNode (name,new_val);
  printNode(new_node);

  //SymbolTable *new_table = createSymbolTable();
  SymbolTable *new_table = NULL;
  Node *lookup = NULL;
  table_insert (&new_table, new_node);
  //Will cause errror// table_insert (new_table, new_node);
  lookup = table_lookup (new_table, new_node);

  printNode(lookup);
  printSymbolTable (new_table);

  freeSymbolTable (new_table);
  return 0;
}