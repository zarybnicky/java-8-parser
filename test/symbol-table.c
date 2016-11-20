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

/*void ReplaceByRight(SymbolTable *table, Node *replace, Node **root){
    if (*root == NULL){
        FERROR(ERR_SEM_UNDEFINED, "Cannot replace object %s", (*root)->symbol);
    }
    else{
        if ((*root)->right != NULL)
            ReplaceByRight(table, replace,&(*root)->right);
        else{
            Node *tmp = table_lookup(table, (*root)->symbol);
            strcpy(replace->symbol, (*root)->symbol);
            replace->type = (*root)->type;
            switch((*root)->type){
                case N_VALUE:
                    replace->data.value = (*root)->data.value;
                    break;
                case N_FUNCTION:
                    replace->data.function = (*root)->data.function;
                    break;
                case N_CLASS:
                    break;
            }
            *root = tmp;
        }
    }
    return;
}
*/
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
  strcpy(name,"a");
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

  strcpy(string,"b");
  new_nd = createValueNode (string,new_v);
//  printNode(new_nd);

  SymbolTable *new_table = createSymbolTable();
  Node *lookup = NULL;

  Value *va = createValue (1);
  va->data.integer = 150;
  Node *a = NULL;

  char *string2;
  string2 = malloc (4* sizeof(char));
  strcpy(string2, "c");
  a = createValueNode (string2,va);

  Value *vb = createValue (1);
  vb->data.integer = 40;
  Node *b = NULL;

  char * string3;
  string3 = malloc (4* sizeof(char));
  strcpy(string3,"d");
  b = createValueNode (string3,vb);

  Value *vc = createValue (1);
  vc->data.integer = 30;
  Node *c = NULL;

  char *string4;
  string4 = malloc (4* sizeof(char));
  string4[0] = 'g';
  string4[1] = '\0';

  c = createValueNode (string4,vc);

  Value *vd = createValue (1);
  vd->data.integer = 20;
  Node *d = NULL;

  char *string5;
  string5 = malloc (4 * sizeof(char));
  strcpy(string5,"h");
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
  //remove root
  lookup = table_remove (new_table, "d");
  printNode(lookup);
  freeNode(lookup);
  //table_iterate (b, pre_order);

  printf("\n");
  printSymbolTable (new_table);

  freeSymbolTable (new_table);
  return 0;
}
