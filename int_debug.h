/*
 * Project: IFJ16, a programming language interpreter
 * FIT VUT Brno
 * Authors: xzaryb00 - Zarybnický Jakub
 *          xtamas01 - Tamaškovič Marek
 *          xvasko12 - Vaško Martin
 *          xvasko14 - Vaško Michal
 *          xzales12 - Záleský Jiří
 */

#ifndef INT_DEBUG
#define INT_DEBUG

// Debug macro for interpret

#ifndef DEBUG
    #define dPrintf(s,...)
#else
    #define dPrintf(s,...) fprintf(stderr, __FILE__": %d: " s "\n",__LINE__,__VA_ARGS__)
#endif

#endif
