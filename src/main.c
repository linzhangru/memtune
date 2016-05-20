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

//unsigned long long get_uptime();






#define NUM_OF_THRESHOLDS 3
#define THRESHOLD_0 1024
#define THRESHOLD_1 896
#define THRESHOLD_2 512
int free_mem_thresholds[NUM_OF_THRESHOLDS] = {
    THRESHOLD_0,
    THRESHOLD_1,
    THRESHOLD_2
};





int select_vm_data(int free)
{
    //TBD: select vm_data according to the free memory status
    //     and eMMC loading ==> not yet finished
    int io_loading_high;
    //int thres;

    io_loading_high = get_io_loading();
    
    if(free/1024 > THRESHOLD_0)
	return 0;
    //trigger page reclaim when io loading is not high
    else if((free/1024 > THRESHOLD_1) && !io_loading_high)
	return 1;
    //trigger page reclaim when io loading is not high
    else if((free/1024 > THRESHOLD_2) && !io_loading_high)
	return 2;
    else
	return 0;  //TBD: consider about more vm configration choices
}




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
