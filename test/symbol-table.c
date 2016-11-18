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

int main(){
  char *name;
  name = malloc (4 * sizeof(char));
  strcpy(name,"h");
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

  char *string;
  string = malloc (4 * sizeof(char));

  strcpy(string,"g");
  new_nd = createValueNode (string,new_v);
//  printNode(new_nd);

  SymbolTable *new_table = createSymbolTable();
  Node *lookup = NULL;

  Value *va = createValue (1);
  va->data.integer = 150;
  Node *a = NULL;

  char *string2;
  string2 = malloc (4* sizeof(char));
  strcpy(string2, "a");
  a = createValueNode (string2,va);

  Value *vb = createValue (1);
  vb->data.integer = 40;
  Node *b = NULL;

  char * string3;
  string3 = malloc (4* sizeof(char));
  strcpy(string3,"b");
  b = createValueNode (string3,vb);

  Value *vc = createValue (1);
  vc->data.integer = 30;
  Node *c = NULL;

  char *string4;
  string4 = malloc (4* sizeof(char));
  string4[0] = 'c';
  string4[1] = '\0';

  c = createValueNode (string4,vc);

  Value *vd = createValue (1);
  vd->data.integer = 20;
  Node *d = NULL;

  char *string5;
  string5 = malloc (4 * sizeof(char));
  strcpy(string5,"d");
  d = createValueNode (string5,vd);


  Value *ve = createValue (1);
  ve->data.integer = 15;
  Node *e = NULL;

  char *string6;
  string6 = (char *) malloc (4 * sizeof(char));
  if (string6 != NULL){
    strcpy(string6,"e");
  }
  e = createValueNode (string6,ve);

  Value *vf = createValue (1);
  vf->data.integer = 11;
  Node *f = NULL;

  char *string7;
  string7 = malloc (4 * sizeof(char));
  strcpy(string7,"f");
  f = createValueNode (string7,vf);

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
  lookup = table_lookup(new_table,"i");
  table_iterate (b, pre_order);

  printf("\n");
  printNode(lookup);
  printf("\n");
  printSymbolTable (new_table);

  freeSymbolTable (new_table);
  return 0;
}
