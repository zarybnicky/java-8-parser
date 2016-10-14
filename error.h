/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef IFJ_ERROR_H
#define IFJ_ERROR_H

#define ERR_LEXER                 1
#define ERR_SYNTAX                2
#define ERR_SEM_UNDEFINED         3
#define ERR_SEM_TYPECHECK         4
#define ERR_SEM_MISC              6
#define ERR_RUNTIME_INT_PARSE     7
#define ERR_RUNTIME_UNINITIALIZED 8
#define ERR_RUNTIME_DIV_BY_ZERO   9
#define ERR_RUNTIME_MISC          10
#define ERR_INTERNAL              99

#endif /* IFJ_ERROR_H */
