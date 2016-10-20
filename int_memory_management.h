/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#include "error.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef INT_MEM_MANAGEMET
#define INT_MEM_MANAGEMET

typedef struct mem_elem {
    struct mem_elem *ptr;
    void* data;
} *mem_elem_ptr;

typedef struct{
    mem_elem_ptr Act;
    mem_elem_ptr First;
    mem_elem_ptr Last;
} mem_t;

void InitMemory (mem_t *L);
void DisposeMemory (mem_t *L);
void *malloc_c (mem_t *L, size_t size);
void *calloc_c (mem_t *L, unsigned num, size_t size);
void free_c (mem_t *L, mem_elem_ptr ptr);
void free_c_all (mem_t *L);

#endif
