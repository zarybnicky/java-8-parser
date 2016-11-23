/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

// Mini garbage collector

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "ir.h"
#include "ial.h"
#include "int_debug.h"

#include "int_memory_management.h"


void test(){
    T_HTable test;
    ht_init (&test);
    print_htTable(&test);
    char *a = malloc_c(sizeof(char));
    char *b = malloc_c(sizeof(char));

    a = '\0';
    b = '\0';

    (void) a;
    (void) b;

    print_htTable(&test);
    free_c(&test, &a);
    print_htTable(&test);
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));

    print_htTable(&test);

    free_c_all(&test);

}

void print_htTable(T_HTable *t){
    for(unsigned i = 0; i < HTAB_SIZE; i++){
        if((*t)[i] != NULL){
            printf("[%d] : ", i);

            T_HTItem *item = (*t)[i];
            while(item != NULL){
                printf(" - [%p]", (void*)&item);
                item = item->next;
            }
            printf("\n");
        }
        else
            continue;
    }
}

void *malloc_c(size_t size){
    if (alloc_tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    void *tmp = malloc(size);

    CHECK_ALLOC(tmp);

    ht_insert(&alloc_tab, &tmp);

    return tmp;
}

void *calloc_c(unsigned num, size_t size){
    if (alloc_tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    void *tmp = calloc(num, size);

    CHECK_ALLOC(tmp);

    ht_insert(&alloc_tab, &tmp);

    return tmp;

}

void free_c(T_HTable *tab, void *addr) {
    if (addr == NULL || tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    int index = hash_function(addr, HTAB_SIZE);

    T_HTItem *item = (*tab)[index];
    T_HTItem *prv = NULL;

    if(item == NULL)
        return;

    while(item->addr != addr){
        prv = item;
        item = item->next;
        if(item == NULL)
            return;
    }


    if(prv != NULL){
        prv->next = item->next;
    }
    else{
        (*tab)[index] = item->next;
    }

    free(item);

}

void free_c_all(T_HTable *tab) {
    if (tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    for(unsigned i = 0; i < HTAB_SIZE; i++){
        T_HTItem *item = (*tab)[i];
        while(item != NULL){
            free_c(tab, (*tab)[i]->addr);
        }
    }

}

void ht_init ( T_HTable *tab ) {

    // tab = NULL;
    for(unsigned i = 0; i < HTAB_SIZE; i++){
        (*tab)[i] = NULL;
    }
}

T_HTItem* ht_search ( T_HTable* tab, void *addr ) {

    int index = hash_function(addr, HTAB_SIZE);

    if(tab != NULL){
        T_HTItem *ptr = (*tab)[index];


        while (ptr != NULL){
            if(ptr->addr == addr)
                return ptr;
            ptr = ptr->next;
        }
    }

    return NULL;
}

void ht_insert ( T_HTable* tab, void *addr ) {

    //check for update
    T_HTItem *tmp = ht_search(tab, addr);
    if (tmp != NULL){
        tmp->addr = addr;
        return;
    }

    // vars
    unsigned index = hash_function(addr, HTAB_SIZE);

    // create new item
    T_HTItem *item = malloc(sizeof(T_HTItem));
    CHECK_ALLOC(item);
    item->addr = addr;

    item->next = (*tab)[index];

    (*tab)[index] = item;
    return;

}

unsigned int hash_function(const void *addr, unsigned htab_size){

    unsigned int h=0;

    // dPrintf("addr: %p",addr);

    char str[42];
    sprintf(str, "%p", addr);


    // dPrintf("str: %s",str);

    const unsigned char *p;

    for( p=(const unsigned char*)str; *p!='\0'; p++ )
        h = 65599*h + *p;

    return h % htab_size;
}

// Mini garbage collector
// END
