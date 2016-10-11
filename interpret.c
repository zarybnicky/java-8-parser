/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "interpret.h"

int main_func(int argc, char *argv[]){

    return 0;
}

int Error(){
    fprintf(stderr,"And he realised, ...\n\t\t... he fucked up");
}

void InitMemory (mem_t *L) {

    if(L == NULL){
        Error();
        return;
    }
    else{
        L->Act = NULL;
        L->First = NULL;
    }

}

/*
void DisposeMemory (mem_t *L) {

    if(L == NULL){
        Error();
        return;
    }

    while(L->Act != NULL && L->First != NULL){
        free(L->First->data);
        mem_elem_ptr buf = L->First->ptr;
        free(L->First);
        L->Act = buf;
        L->First = buf;
    }

    L->Act = NULL;
    L->First = NULL;

}
*/

void *malloc_c (mem_t *L, size_t size) {

    if(L == NULL){
        Error();
        return NULL;
    }

    mem_elem_ptr tmp = malloc(sizeof(struct mem_elem));

    if(tmp == NULL){
        Error();
        return NULL;
    }

    tmp->data = malloc(size);

    if(tmp->data == NULL){
        Error();
        return NULL;
    }

    tmp->ptr = L->First;
    L->First = tmp;

    return tmp;

}

void *calloc_c (mem_t *L, unsigned num, size_t size) {

    if(L == NULL){
        Error();
        return NULL;
    }

    mem_elem_ptr tmp = malloc(sizeof(struct mem_elem));

    if(tmp == NULL){
        Error();
        return NULL;
    }

    tmp->data = calloc(num, size);

    if(tmp->data == NULL){
        Error();
        return NULL;
    }

    tmp->ptr = L->First;
    L->First = tmp;

    return tmp;

}

void DeleteItem (mem_t *L, mem_elem_ptr ptr){

    if(ptr == NULL || L == NULL){
        fprintf(stderr, "%s\n", "NOPENOPENOPE");
    }

    if(ptr == L->First){
        L->First = L->First->ptr;
    }
    else{
        for(mem_elem_ptr tmp = L->First, prev = NULL ;
            tmp != ptr;
            ptr = ptr->ptr){


                if(tmp == ptr){
                    prev->ptr = ptr->ptr;
                    break;
                }

                prev = ptr;
        }
    }

    free(ptr->data);
    free(ptr);
    return;


}

void DisposeMemory (mem_t *L) {
    if(L == NULL){
        Error();
        return;
    }

    while(L->Act != NULL && L->First != NULL){
        mem_elem_ptr buf = L->First->ptr;
        free(L->First->data);
        free(L->First);
        L->Act = buf;
        L->First = buf;
    }

    L->Act = NULL;
    L->First = NULL;

}
