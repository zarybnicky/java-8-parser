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
#include "int_memory_management.h"


// MEMORY MANAGEMENT TESTING
void test_mem(){
    ht_init (&alloc_tab);
    //printf("\n\n-------------\n%s\n-------------\n", "1. prazdna tab" );
    print_htTable(&alloc_tab);


    char *a = malloc_c(sizeof(char));
    char *b = malloc_c(sizeof(char));

    // printf("p: %p", (void*)(*alloc_tab)[16644].addr);

    printf("\n\n-------------\n%s\n-------------\n", "2. dva prvky" );
    print_htTable(&alloc_tab);


    *a = '2';
    *b = '2';

    (void) a;
    (void) b;

    printf("\n\n-------------\n%s\n-------------\n", "3. dva prvky" );
    print_htTable(&alloc_tab);

    free_c(a);
    printf("\n\n-------------\n%s\n-------------\n", "4. jeden vymazem" );
    print_htTable(&alloc_tab);

    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));
    a = malloc_c(sizeof(char));

    printf("\n\n-------------\n%s\n-------------\n", "5. 6 prvkou" );
    print_htTable(&alloc_tab);



    free_c_all(&alloc_tab);
    printf("\n\n-------------\n%s\n-------------\n", "6. prazdna tab" );
    print_htTable(&alloc_tab);

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


// custom malloc for our memory management
void *malloc_c(size_t size){
    if (alloc_tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    void *tmp = malloc(size);

    CHECK_ALLOC(tmp);

    ht_insert(&alloc_tab, tmp);

    return tmp;
}


// custom calloc for our memory management
void *calloc_c(unsigned num, size_t size){
    if (alloc_tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    void *tmp = calloc(num, size);

    CHECK_ALLOC(tmp);

    ht_insert(&alloc_tab, tmp);

    return tmp;

}

// custom realloc for our memory management
void *realloc_c (void *ptr, size_t size){
    if (alloc_tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    ht_del_item(&alloc_tab, ptr);

    ptr = realloc(ptr, size);

    CHECK_ALLOC(ptr);

    ht_insert(&alloc_tab, ptr);

    return ptr;
}

// delete item in memory management
void ht_del_item(T_HTable *tab, void *addr){

    unsigned index = hash_function(addr, HTAB_SIZE);

    if(tab != NULL){
        T_HTItem *ptr = (*tab)[index];
        T_HTItem *tmp = ptr;

        while (ptr != NULL){

            if (ptr->addr == addr){

                // First in order
                if (ptr == tmp){
                    (*tab)[index] = ptr->next;
                    free(ptr);
                    return;
                }

                tmp->next = ptr->next;
                free(ptr);
                return;
            }

            tmp = ptr;
            ptr = ptr->next;
        }

    }

}

// custom free for our memory management
void free_c(void *addr) {
    if (addr == NULL || alloc_tab == NULL) {
        return;
    }

    unsigned index = hash_function(addr, HTAB_SIZE);

    T_HTItem *item = alloc_tab[index];
    T_HTItem *prv = NULL;
    // First item hit
    if (item == NULL)
        return;
    //loop between items find exact address for 1 table-item
    while(item->addr != addr){
        prv = item;
        item = item->next;
        /* no next item found just return; */
        if (item == NULL)
            return;
        if (item->addr == addr){
            prv->next = item->next;
            free(addr);
            free(item);
            item = NULL;
            return;
        }
    }
    // first node
    alloc_tab[index] = item->next;
    free(addr);
    free(item);
    item = NULL;
}

// Release the KRAK.. ehm... MEMORYY!
void free_c_all(T_HTable *tab) {
    if (tab == NULL) {
        ERROR(ERR_INTERNAL);
    }

    T_HTItem *tmp;
    T_HTItem *item=(*tab)[0];

    for(unsigned i = 0; i < HTAB_SIZE; i++, item = (*tab)[i]){
        while(item != NULL){
            tmp = item->next;
            free(item->addr);
            free(item);
            item = tmp;
        }

        (*tab)[i] = NULL;
    }
}

// Initializer for memory management
void ht_init ( T_HTable *tab ) {
    for(unsigned i = 0; i < HTAB_SIZE; i++){
        (*tab)[i] = NULL;
    }
}

// Search address in memory management
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

// insert address in memory management
void ht_insert ( T_HTable* tab, void *addr ) {
    unsigned index = hash_function(addr, HTAB_SIZE);

    // create new item
    T_HTItem *item = malloc(sizeof(T_HTItem));
    CHECK_ALLOC(item);
    item->addr = addr;

    /* Link item with table index */
    item->next = (*tab)[index];
    (*tab)[index] = item;
    return;

}

// hash function for mem. management
unsigned hash_function(const void *addr, unsigned htab_size){

    assert(addr != NULL);

    unsigned int h=0;

    char str[42];
    sprintf(str, "%p", addr);

    char *p;

    for( p=(char*)str; *p!='\0'; p++ )
        h = 65599 * h + (*p);

    return (h % htab_size);
}

// Mini garbage collector
// END
