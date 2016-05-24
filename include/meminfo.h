#ifndef __MEMINFO__
#define __MEMINFO__


#include "dtype.h"


//SIZE_MEMINFO is defined to be result of "cat /proc/meminfo | wc -l"
#if defined(_X86_)
odefine SIZE_MEMINFO 45
#elif defined(_AARCH64_)
#define SIZE_MEMINFO 37
#else
#error "not supprted architecture"
#endif

#define INVALID_VAL  (-1)

#if defined(_X86_)
enum {
    MEMTOTAL,
    MEMFREE,
    MEMAVAILABLE,
    BUFFERS,
    CACHED,
    SWAPCACHED
};
#elif defined(_AARCH64_)
enum {
    MEMTOTAL,
    MEMFREE,
    BUFFERS,
    CACHED,
    SWAPCACHED
};
#else
#error "not supprted architecture"
#endif


extern int parse_meminfo();
extern struct entry meminfo[SIZE_MEMINFO];

#endif
