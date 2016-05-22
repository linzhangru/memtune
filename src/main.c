#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "meminfo.h"
#include "ioloading.h"
#include "sysctlvm.h"




void init()
{
    iostat_init();
}



int main()
{
    unsigned long free;
    int choice;
    int sec = 0;


    init();
    sleep(1);
    
#if 1
    while(1) {
	printf("============================\n"
	       "seconds:%d, choice:%d\n", sec++, choice);
	parse_meminfo();
	free = meminfo[MEMFREE].val + meminfo[CACHED].val;
	printf("free: %ld(F) + %ld(C) = %ld MB\n", meminfo[MEMFREE].val/1024, meminfo[CACHED].val/1024, free/1024);
	choice = select_vm_data(free);
	reconfig_vmfds(choice);    
	get_vm_data();
	sleep(1);
	
    }
#endif    
    return 0;
}
