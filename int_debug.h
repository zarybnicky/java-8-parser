#ifndef INT_DEBUG
#define INT_DEBUG

#ifdef NDEBUG
    #define dPrintf(s,...)
#else
    #define dPrintf(s,...) fprintf(stderr, __FILE__":%u:" s "\n",__LINE__,__VA_ARGS__)
#endif

#endif
