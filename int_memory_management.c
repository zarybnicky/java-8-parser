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

// FIXME this ING file has so MANY errors

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "ir.h"
#include "ial.h"

#include "int_memory_management.h"

#ifdef NDEBUG
    #define dPrintf(s,...)
#else
    #define dPrintf(s,...) fprintf(stderr, __FILE__":%u:" s "\n",__LINE__,__VA_ARGS__)
#endif


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

void free_c(void *addr) {
    if (addr == NULL || alloc_tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    int index = hash_function(addr, HTAB_SIZE);

    T_HTItem *item = &(*alloc_tab)[index];
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
        alloc_tab[index] = item->next;
    }

    free(item);

}

void free_c_all(void) {
    if (alloc_tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    for(int i = 0; i < HTAB_SIZE; i++){
        while(alloc_tab[i] != NULL){
            free_c(alloc_tab[i]->addr);
        }
    }

}

void ht_init ( T_HTable *tab ) {

    // tab = NULL;
    for(int i = 0; i < HTAB_SIZE; i++){
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
    unsigned int index = hash_function(addr, HTAB_SIZE);

    // create new item
    T_HTItem *item = malloc(sizeof(T_HTItem));
    item->addr = addr;

    item->next = (*tab)[index];

    (*tab)[index] = item;
    return;

}

unsigned int hash_function(const void *addr, unsigned htab_size){

    unsigned int h=0;

    dPrintf("addr: %p",addr);

    char str[42];
    sprintf(str, "%p", addr);


    dPrintf("str: %s",str);

    const unsigned char *p;

    for( p=(const unsigned char*)str; *p!='\0'; p++ )
        h = 65599*h + *p;

    return h % htab_size;
}

// Mini garbage collector
// END
