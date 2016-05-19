#ifndef __MEMINFO__
#define __MEMINFO__


struct entry
{
    const char* name;
    long  val;
};

#define SIZE_MEMINFO 45
#define INVALID_VAL  (-1)

enum {
    MEMTOTAL,
    MEMFREE,
    MEMAVAILABLE,
    BUFFERS,
    CACHED,
    SWAPCACHED
};


extern int parse_meminfo();
extern struct entry meminfo[SIZE_MEMINFO];

#endif
