#ifndef INT_DEBUG
#define INT_DEBUG

#ifndef DEBUG
    #define dPrintf(s,...)
#else
    #define dPrintf(s,...) fprintf(stderr, __FILE__": %d: " s "\n",__LINE__,__VA_ARGS__)
#endif

#endif
