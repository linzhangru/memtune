#ifndef __SYSCTLVM__
#define __SYSCTLVM__

#include "dtype.h"

enum {
    DIRTY_EXPIRE_CENTISECS = 0,
    DIRTY_WRITEBACK_CENTISECS,
    DIR_RATIO,
    DIRTY_BACKGROUND_RATIO,
    MIN_FREE_KBYTES,
    VFS_CACHE_PRESSURE,
    SWAPPINESS,
    NR_FDS
};


extern int get_vm_data();
extern int reconfig_vmfds(int choice);

#endif
