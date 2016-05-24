#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "meminfo.h"
#include "ioloading.h"
#include "sysctlvm.h"



/*
/proc/sys/vm/dirty_expire_centisecs:    3000 -->  2000 -->  1000
/proc/sys/vm/dirty_writeback_centisecs:  500 -->   400 -->   300
/proc/sys/vm/dirty_ratio:                 20 -->    15 -->    10
/proc/sys/vm/dirty_background_ratio:      10 -->    20 -->    30 
/proc/sys/vm/min_free_kbytes:           5624 --> 28120 --> 56240 
/proc/sys/vm/vfs_cache_pressure:         100 -->   150 -->   200
/proc/sys/vm/swappiness:                  60 -->    65 -->    70
*/

#if 0
long config_val[3][7] = {
    {3000, 500, 20, 10,  5624, 100, 60},
    {2000, 400, 15, 20, 28120, 150, 65},
    {1000, 300, 10, 30, 56240, 200, 70},
};
#else
long config_val[3][7] = {
    {3000, 500, 20, 10, 5624, 100, 60},
    {3000, 400, 15, 20, 5624, 100, 60},
    {3000, 300, 10, 30, 5624, 100, 60},
};
#endif

    

struct entry vmfds[NR_FDS] = {
    {"/proc/sys/vm/dirty_expire_centisecs",    -1},
    {"/proc/sys/vm/dirty_writeback_centisecs", -1},
    {"/proc/sys/vm/dirty_ratio",               -1},
    {"/proc/sys/vm/dirty_background_ratio",    -1},
    {"/proc/sys/vm/min_free_kbytes",           -1},
    {"/proc/sys/vm/vfs_cache_pressure",        -1},
    {"/proc/sys/vm/swappiness",                -1}
};




int get_vm_data()
{
    char data[64];
    long val;
    int i;

    for(i = 0; i < NR_FDS; i++){
	vmfds[i].val = open(vmfds[i].name, O_RDWR);
	if(vmfds[i].val < 0){
	    printf("fail to open file %s: %ld\n", vmfds[i].name, vmfds[i].val);
	    return -1;//FIXME: file should be closed before return
	}
	//printf("%s:%ld\n", vmfds[i].name, vmfds[i].val);
    }
    //printf("%s all vm files open succeeded\n", __func__);

    
    for(i = 0; i < NR_FDS; i++){
	memset(data, 0, 64);
	//fscanf(vmfds[i].val, "%d", &val);
	if(read(vmfds[i].val, data, 64) <= 0){
	    printf("file %s reads failure\n", vmfds[i].name);
	    return -1;//FIXME: file should be closed before return
	}
	sscanf(data, "%ld", &val);
	printf(">>%s: %ld\n", vmfds[i].name+13, val);
    }

    for(i = 0; i < NR_FDS; i++){    
	if(vmfds[i].val >= 0)
	    close(vmfds[i].val);
    }
    //printf("%s all vm files closed\n", __func__);
    return 0;
}



int reconfig_vmfds(int choice)
{
    char data[64];
    int i;
    long * newvals = config_val[choice];
    
    //TBD: inject new config datas through sysctl.vm
    for(i = 0; i < NR_FDS; i++){
	vmfds[i].val = open(vmfds[i].name, O_RDWR);
	if(vmfds[i].val < 0){
	    printf("fail to open file %s: %ld\n", vmfds[i].name, vmfds[i].val);
	    return -1;//FIXME: file should be closed before return
	}
	//printf("%s:%ld\n", vmfds[i].name, vmfds[i].val);
    }
    //printf("%s all vm files open succeeded\n", __func__);


    for(i = 0; i < NR_FDS; i++){
	memset(data, 0, 64);
	snprintf(data, sizeof(newvals[i]), "%ld\n", newvals[i]);
	if(write(vmfds[i].val, data, sizeof(data)) <= 0){
	    printf("file %s writes failure\n", vmfds[i].name);
	    return -1; //FIXME: file should be closed before return
	}
	//printf("_%s: %ld\n", vmfds[i].name, val);
    }    
    

    for(i = 0; i < NR_FDS; i++){    
	if(vmfds[i].val >= 0)
	    close(vmfds[i].val);
    }
    //printf("%s all vm files closed\n", __func__);
    return 0;
}



#define NUM_OF_THRESHOLDS 3
#define THRESHOLD_0 1024
#define THRESHOLD_1 896
#define THRESHOLD_2 512

enum choice_t {
    CHOICE_0,
    CHOICE_1,
    CHOICE_2
};




int select_vm_data(int free)
{
    //TBD: select vm_data according to the free memory status
    //     and eMMC loading ==> not yet finished
#if 0    
    int io_loading_high;
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
#else
    enum choice_t selection = CHOICE_0;
    int val;
    char data[32];
    int fd = open("selection", O_RDONLY);
    if(fd < 0){
	printf("fail to open file selection\n");
	return CHOICE_0;
    }
    if(read(fd, data, sizeof(data)) < 0){
	goto out;
    }
    sscanf(data, "%d", &val);
    if(val > CHOICE_2 || val < CHOICE_0){
	printf("wrong selection val, pls change val in file selection between [%d, %d]\n", CHOICE_0, CHOICE_2);
	selection = CHOICE_0;
    } else {
	selection = val;
    }
    
    printf("selection : %d\n", selection);
out:    
    close(fd);
    return selection;
#endif
    
}

