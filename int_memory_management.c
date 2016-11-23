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
#include <limits.h>

#include "error.h"
#include "ir.h"
#include "ial.h"
#include "int_debug.h"
#include "assert.h"

#include "int_memory_management.h"

T_HTable test_table_asd;

void test_mem(){
    ht_init (&test_table_asd);
    //printf("\n\n-------------\n%s\n-------------\n", "1. prazdna tab" );
    print_htTable(&test_table_asd);


    char *a = malloc_c(sizeof(char));
    char *b = malloc_c(sizeof(char));

    // printf("p: %p", (void*)(*test_table_asd)[16644].addr);

    printf("\n\n-------------\n%s\n-------------\n", "2. dva prvky" );
    print_htTable(&test_table_asd);


    *a = '2';
    *b = '2';

    (void) a;
    (void) b;

    printf("\n\n-------------\n%s\n-------------\n", "3. dva prvky" );
    print_htTable(&test_table_asd);

    free_c(&test_table_asd, a);
    printf("\n\n-------------\n%s\n-------------\n", "4. jeden vymazem" );
    print_htTable(&test_table_asd);

    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));

    printf("\n\n-------------\n%s\n-------------\n", "5. 6 prvkou" );
    print_htTable(&test_table_asd);



    free_c_all(&test_table_asd);
    printf("\n\n-------------\n%s\n-------------\n", "6. prazdna tab" );
    print_htTable(&test_table_asd);

    printf("\n\n----------%s----------", "koniec" );




}

void print_htTable(T_HTable *t){
    for(unsigned i = 0; i < HTAB_SIZE; i++){
        if((*t)[i] != NULL){
            printf("[%d] : ", i);

            T_HTItem *item = (*t)[i];
            while(item != NULL){
                printf(" -> [%p]", (void*)(item->addr));
                item = item->next;
            }
            printf("\n");
        }
    }
}

void *malloc_c(size_t size){
    if (test_table_asd == NULL) {
        ERROR(ERR_INTERNAL);
    }

    void *tmp = malloc(size);

    CHECK_ALLOC(tmp);

    ht_insert(&test_table_asd, tmp);

    return tmp;
}

void *calloc_c(unsigned num, size_t size){
    if (alloc_tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    void *tmp = calloc(num, size);

    CHECK_ALLOC(tmp);

    ht_insert(&alloc_tab, tmp);

    return tmp;

}

void free_c(T_HTable *tab, void *addr) {
    printf("free_c\naddr: %p\n", addr);
    if (addr == NULL || tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    unsigned index = hash_function(addr, HTAB_SIZE);
    printf("index: %u\n", index);

    T_HTItem *item = (*tab)[index];

    if(item != NULL && item->addr == addr){
        (*tab)[index] = item->next;
        free(addr);
        free(item);
        return;
    }
    else
        return;

    // loop
    T_HTItem *prv = item;
    while(item != NULL){
        item = item->next;
        if (item->addr == addr){
            prv->next = item->next;
            free(addr);
            free(item);
            return;
        }
    }


    //if(prv != NULL){
    //    prv->next = item->next;
   // }
   // else{
     //   (*tab)[index] = item->next;
    //}
}

void free_c_all(T_HTable *tab) {
    if (tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    for(unsigned i = 0; i < HTAB_SIZE; i++){
        T_HTItem *item = (*tab)[i];
        if(item != NULL){
            free_c(tab, item->addr);
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

    unsigned index = hash_function(addr, HTAB_SIZE);

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
    // T_HTItem *tmp = ht_search(tab, addr);
    // if (tmp != NULL){
    //     // tmp->addr = addr;
    //     return;
    // }

    // vars
    unsigned index = hash_function(addr, HTAB_SIZE);

    // create new item
    T_HTItem *item = malloc(sizeof(T_HTItem));
    CHECK_ALLOC(item);
    item->addr = addr;

    item->next = (*tab)[index];

    (*tab)[index] = item;

    // printf("NULL: %p, p: %p, index: %u\n", NULL, (void*)(*tab)[index], index);

    return;

}

unsigned hash_function(const void *addr, unsigned htab_size){

    assert(addr != NULL);

    unsigned int h=0;

    // dPrintf("addr: %p",addr);
    char str[42];
    sprintf(str, "%p", addr);

    // dPrintf("str: %s",str);
    char *p;

    for( p=(char*)str; *p!='\0'; p++ )
        h = 65599 * h + (*p);

    // printf("(%u)/(%u)\n", h, htab_size);
    // printf("%u\n", h%htab_size);

    return (h % htab_size);
}

// Mini garbage collector
// END
