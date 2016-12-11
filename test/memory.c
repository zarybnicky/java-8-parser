/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "../int_memory_management.h"

void freeGlobalResources() { }

void print_htTable(T_HTable *t){
    for(unsigned i = 0; i < HTAB_SIZE; i++){
        if((*t)[i] == NULL)
            continue;

        printf("[%u]", i);

        T_HTItem *item = (*t)[i];
        while(item != NULL){
            printf(" -> [%p]", (void *) (item->addr));
            item = item->next;
        }
        printf("\n");
    }
}

int main() {
    ht_init(&alloc_tab);
    print_htTable(&alloc_tab);

    char *a = malloc_c(sizeof(char));
    char *b = malloc_c(sizeof(char));

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

    malloc_c(sizeof(char));
    malloc_c(sizeof(char));
    malloc_c(sizeof(char));
    malloc_c(sizeof(char));
    malloc_c(sizeof(char));

    printf("\n\n-------------\n%s\n-------------\n", "5. 6 prvkou" );
    print_htTable(&alloc_tab);

    free_c_all(&alloc_tab);
    printf("\n\n-------------\n%s\n-------------\n", "6. prazdna tab" );
    print_htTable(&alloc_tab);

    printf("\n\n----------%s----------", "koniec" );

    return 0;
}
