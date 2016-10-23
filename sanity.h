/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_SANITY_H
#define IFJ_SANITY_H

#include <stdlib.h>
#include "error.h"
#include "ir.h"
#include "ial.h"

void fcheck(Node *node);
void sancheck(Node *node);

#endif /* IFJ_SANITY_H */
