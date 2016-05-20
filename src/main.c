#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "meminfo.h"
#include "ioloading.h"

unsigned long long get_uptime();



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



/*
/proc/sys/vm/dirty_expire_centisecs:    3000 -->  2000 -->  1000
/proc/sys/vm/dirty_writeback_centisecs:  500 -->   400 -->   300
/proc/sys/vm/dirty_ratio:                 20 -->    15 -->    10
/proc/sys/vm/dirty_background_ratio:      10 -->    20 -->    30 
/proc/sys/vm/min_free_kbytes:           5624 --> 28120 --> 56240 
/proc/sys/vm/vfs_cache_pressure:         100 -->   150 -->   200
/proc/sys/vm/swappiness:                  60 -->    65 -->    70
*/

long config_val[3][7] = {
    {3000, 500, 20, 10,  5624, 100, 60},
    {2000, 400, 15, 20, 28120, 150, 65},
    {1000, 300, 10, 30, 56240, 200, 70},
};

    

struct entry vmfds[NR_FDS] = {
    {"/proc/sys/vm/dirty_expire_centisecs",    -1},
    {"/proc/sys/vm/dirty_writeback_centisecs", -1},
    {"/proc/sys/vm/dirty_ratio",               -1},
    {"/proc/sys/vm/dirty_background_ratio",    -1},
    {"/proc/sys/vm/min_free_kbytes",           -1},
    {"/proc/sys/vm/vfs_cache_pressure",        -1},
    {"/proc/sys/vm/swappiness",                -1}
};



#define NUM_OF_THRESHOLDS 3
#define THRESHOLD_0 1024
#define THRESHOLD_1 896
#define THRESHOLD_2 512
int free_mem_thresholds[NUM_OF_THRESHOLDS] = {
    THRESHOLD_0,
    THRESHOLD_1,
    THRESHOLD_2
};



int get_vm_data(struct entry vmfds[])
{
    char data[64];
    long val;

    for(int i; i < NR_FDS; i++){
	vmfds[i].val = open(vmfds[i].name, O_RDWR);
	if(vmfds[i].val < 0){
	    printf("fail to open file %s: %ld\n", vmfds[i].name, vmfds[i].val);
	    return -1;//FIXME: file should be closed before return
	}
	//printf("%s:%ld\n", vmfds[i].name, vmfds[i].val);
    }
    printf("all vm files open succeeded\n");

    
    for(int i; i < NR_FDS; i++){
	memset(data, 0, 64);
	//fscanf(vmfds[i].val, "%d", &val);
	if(read(vmfds[i].val, data, 64) <= 0){
	    printf("file %s reads failure\n", vmfds[i].name);
	    return -1;//FIXME: file should be closed before return
	}
	sscanf(data, "%ld", &val);
	printf("_%s: %ld\n", vmfds[i].name, val);
    }

    for(int i; i < NR_FDS; i++){    
	if(vmfds[i].val >= 0)
	    close(vmfds[i].val);
    }
    printf("all vm files closed\n");
    return 0;
}



int reconfig_vmfds(long * newvals)
{
    char data[64];
    //long val;
    
    //TBD: inject new config datas through sysctl.vm
    for(int i; i < NR_FDS; i++){
	vmfds[i].val = open(vmfds[i].name, O_RDWR);
	if(vmfds[i].val < 0){
	    printf("fail to open file %s: %ld\n", vmfds[i].name, vmfds[i].val);
	    return -1;//FIXME: file should be closed before return
	}
	//printf("%s:%ld\n", vmfds[i].name, vmfds[i].val);
    }
    printf("all vm files open succeeded\n");


    for(int i; i < NR_FDS; i++){
	memset(data, 0, 64);
	snprintf(data, sizeof(newvals[i]), "%ld\n", newvals[i]);
	if(write(vmfds[i].val, data, sizeof(data)) <= 0){
	    printf("file %s writes failure\n", vmfds[i].name);
	    return -1; //FIXME: file should be closed before return
	}
	//printf("_%s: %ld\n", vmfds[i].name, val);
    }    
    

    for(int i; i < NR_FDS; i++){    
	if(vmfds[i].val >= 0)
	    close(vmfds[i].val);
    }
    printf("all vm files closed\n");
    return 0;
}




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
	reconfig_vmfds(config_val[choice]);    
	get_vm_data(vmfds);
	sleep(1);
	
    }
#endif    
    return 0;
}
