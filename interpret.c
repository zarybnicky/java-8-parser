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

void InitMemory(mem_t *L) {
    if (L == NULL) {
        ERROR(ERR_INTERNAL);
    }
    L->Act = NULL;
    L->First = NULL;
}

void *malloc_c(mem_t *L, size_t size){
    if (L == NULL) {
        ERROR(ERR_INTERNAL);
    }
    mem_elem_ptr tmp = malloc(sizeof(struct mem_elem));
    CHECK_ALLOC(tmp);
    tmp->data = malloc(size);
    CHECK_ALLOC_(tmp->data, free(tmp));

    tmp->ptr = L->First;
    L->First = tmp;
    return tmp;
}

void *calloc_c(mem_t *L, unsigned num, size_t size){
    if (L == NULL) {
        ERROR(ERR_INTERNAL);
    }
    mem_elem_ptr tmp = malloc(sizeof(struct mem_elem));
    CHECK_ALLOC(tmp);
    tmp->data = calloc(num, size);
    CHECK_ALLOC_(tmp->data, free(tmp));

    tmp->ptr = L->First;
    L->First = tmp;
    return tmp;
}

void DeleteItem(mem_t *L, mem_elem_ptr ptr) {
    if (ptr == NULL || L == NULL) {
        ERROR(ERR_INTERNAL);
    }
    if (ptr == L->First) {
        L->First = L->First->ptr;
    } else {
        for (mem_elem_ptr tmp = L->First, prev = NULL; tmp != ptr; ptr = ptr->ptr) {
            if (tmp == ptr) {
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

void DisposeMemory(mem_t *L) {
    if (L == NULL) {
        ERROR(ERR_INTERNAL);
    }
    while (L->Act != NULL && L->First != NULL) {
        mem_elem_ptr buf = L->First->ptr;
        free(L->First->data);
        free(L->First);
        L->Act = buf;
        L->First = buf;
    }

    L->Act = NULL;
    L->First = NULL;
}
