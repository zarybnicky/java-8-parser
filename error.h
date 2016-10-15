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

typedef enum {
    ERR_LEXER                 = 1,
    ERR_SYNTAX                = 2,
    ERR_SEM_UNDEFINED         = 3,
    ERR_SEM_TYPECHECK         = 4,
    ERR_SEM_MISC              = 6,
    ERR_RUNTIME_INT_PARSE     = 7,
    ERR_RUNTIME_UNINITIALIZED = 8,
    ERR_RUNTIME_DIV_BY_ZERO   = 9,
    ERR_RUNTIME_MISC          = 10,
    ERR_INTERNAL              = 99
} ErrorType;

#define EXPLAIN_ERR(type)                                               \
    "Unspecified error %s",                                             \
    (type == ERR_LEXER ? "during lexical analysis" :                    \
     type == ERR_SYNTAX ? "during syntactic analysis" :                 \
     type == ERR_SEM_UNDEFINED ? "while checking for undefined variables" : \
     type == ERR_SEM_TYPECHECK ? "while typechecking" :                 \
     type == ERR_SEM_MISC ? "during semantic analysis" :                \
     type == ERR_RUNTIME_INT_PARSE ? "while parsing a number from stdin" : \
     type == ERR_RUNTIME_UNINITIALIZED ? "- using an uninitialized variable" : \
     type == ERR_RUNTIME_DIV_BY_ZERO ? "- dividing by zero" :           \
     type == ERR_RUNTIME_MISC ? "during program execution" :            \
     "(internal)")

extern void freeGlobalResources();

#define ERROR(ret) do {                                 \
        fprintf(stderr, "Aborting at %s:%d in %s...\n", \
                __FILE__, __LINE__, __func__);          \
        fprintf(stderr, EXPLAIN_ERR(ret));              \
        exit(ret);                                      \
    } while (0);

#define MERROR(ret, fmt) do {                           \
        fprintf(stderr, "Aborting at %s:%d in %s...\n", \
                __FILE__, __LINE__, __func__);          \
        fprintf(stderr, fmt);                           \
        exit(ret);                                      \
    } while (0);

#define FERROR(ret, fmt, ...) do {                      \
        fprintf(stderr, "Aborting at %s:%d in %s...\n", \
                __FILE__, __LINE__, __func__);          \
        fprintf(stderr, fmt, ## __VA_ARGS__);           \
        exit(ret);                                      \
    } while (0);

#define PERROR(msg) do {                                     \
        fprintf(stderr, "Aborting at %s:%d in %s...\n",      \
                __FILE__, __LINE__, __func__);               \
        perror(msg);                                         \
        exit(ERR_INTERNAL);                                  \
    } while (0);

#define PERROR_(msg, ...) do {                               \
        fprintf(stderr, "Aborting at %s:%d in %s...\n",      \
                __FILE__, __LINE__, __func__);               \
        perror(msg);                                         \
        __VA_ARGS__;                                         \
        exit(ERR_INTERNAL);                                  \
    } while (0);

#define CHECK_ALLOC(x)                              \
    if ((x) == NULL) {                              \
        PERROR("Error while allocating memory");    \
    }

#define CHECK_ALLOC_(x, ...)                                    \
    if ((x) == NULL) {                                          \
        PERROR_("Error while allocating memory", __VA_ARGS__);  \
    }

#endif /* IFJ_ERROR_H */
