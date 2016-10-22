/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tama¹koviè Marek
 *          xvasko12 - Va¹ko Martin
 *          xvasko14 - Va¹ko Michal
 *          xzales12 - Záleský Jiøí
 */

// table_iterate(&root, sancheck());

#include "stdbool.h"
#include "ial.h"
#include "ir.h"

void fcheck(Node *node){

    int i;

    // ARGS num
    Declaration *pom = node->data.function->argHead->next;

    for (i = 0; pom != NULL; i++){
        pom = pom->next;
    }

    if (node->data.function->argCount != i){
        MERROR(ERR_SEM_TYPECHECK, "Function has different number of arguments!");
    }

    // Return

    Command *comm = node->data.function->body.tail;
    bool ret = false;

    if (comm != NULL){
	
        do {
	    
            if ((comm->type == C_RETURN) && (node->data.function->returnType != T_VOID)){
		
                if (comm->data.expr->data.value->type != node->data.function->returnType){
                    MERROR(ERR_SEM_TYPECHECK, "Trying to return different type than declared!");
		}
	        ret = true;
		
	    }else if ((comm->type == C_RETURN) && (node->data.function->returnType == T_VOID)){

                    MERROR(ERR_SEM_TYPECHECK, "Trying to return value in void function!")
	    }
	    
        }while (comm->next != NULL);

	    if ((node->data.function->returnType != T_VOID) && (ret ==false)){
                 MERROR(ERR_SEM_TYPECHECK, "Missing return value in function!") 
	    }
    
    }

    
}

void sancheck(Node *node){
    
    static bool first = true;
    
    if (first == true){
   //     Node *root = node;

        first = false;  
    }
//root = NULL; 
    if (node->type == N_VALUE){
	// Variable check
    }

    if (node->type == N_FUNCTION){
        fcheck(node);
    }

    
}


