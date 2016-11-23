/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef INT_MEM_MANAGEMET
#define INT_MEM_MANAGEMET

#define HTAB_SIZE 49157

typedef struct T_HTItem{
    ValueType type;
    void *addr;
    struct T_HTItem *next;
} T_HTItem;

typedef T_HTItem* T_HTable[HTAB_SIZE];

T_HTable alloc_tab;

void *malloc_c (size_t size);
void *calloc_c (unsigned num, size_t size);
void free_c (T_HTable *, void *addr);
void free_c_all(T_HTable *);
unsigned hash_function(const void *, unsigned);
void ht_init ( T_HTable *tab );
T_HTItem* ht_search ( T_HTable* tab, void *addr );
void ht_insert ( T_HTable* tab, void *addr );

void test_mem();
void print_htTable(T_HTable *);

#endif
