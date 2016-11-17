#include "../ir.h"
#include "../ial.h"

void pre_order(Node *node){
  if (node != NULL){
    printNode(node);
    pre_order(node->left);
    pre_order(node->right);
  }
}

int main(){
  char name[10] ="a";
  Value *new_val = createValue (1);
  new_val->data.integer = 10;
  printValue (new_val);
  printf("\n");

  Node *new_node = NULL;
  new_node = createValueNode (name,new_val);
  //printNode(new_node);

  Value *new_v = createValue (1);
  new_v->data.integer = 10;
  // TODO same value free problem!! ir.c
  Node *new_nd = NULL;
  new_nd = createValueNode ("b",new_v);
//  printNode(new_nd);

  SymbolTable *new_table = createSymbolTable();
  Node *lookup = NULL;

  Value *va = createValue (1);
  va->data.integer = 10;
  Node *a = NULL;
  a = createValueNode ("c",va);

  Value *vb = createValue (1);
  vb->data.integer = 10;
  Node *b = NULL;
  b = createValueNode ("d",vb);

  Value *vc = createValue (1);
  vc->data.integer = 10;
  Node *c = NULL;
  c = createValueNode ("e",vc);

  Value *vd = createValue (1);
  vd->data.integer = 10;
  Node *d = NULL;
  d = createValueNode ("f",vd);


  Value *ve = createValue (1);
  ve->data.integer = 10;
  Node *e = NULL;
  e = createValueNode ("i",ve);

  Value *vf = createValue (1);
  vf->data.integer = 10;
  Node *f = NULL;
  f = createValueNode ("j",vf);

  table_insert (new_table, new_node);
  table_insert (new_table, new_nd);
  table_insert (new_table, a);
  table_insert (new_table, b);
  table_insert (new_table, c);
  table_insert (new_table, d);
  table_insert (new_table, e);
  table_insert (new_table, f);

  // returns null lookup
  lookup = table_lookup (NULL, "");
  table_iterate (b, pre_order);

  printf("\n");
  printNode(lookup);
  printf("\n");
  printSymbolTable (new_table);

  freeSymbolTable (new_table);
  return 0;
}
