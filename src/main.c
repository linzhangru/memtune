#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "meminfo.h"

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
}



int reconfig_vmfds(long * newvals)
{
    char data[64];
    long val;
    
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
    
}


//TBD: "/sys/block/sdb/stat" is to be replaced by  "/sys/block/mmcblk0"
//     they have the same out put format:
#define EMMC_DEV_STAT "/sys/block/sdb/stat"   //"/sys/block/mmcblk0"
struct io_stat {
    int num_ird;    //1.number of issued reads
    int num_rdm;    //2.number of reads merged
    int num_scr;    //3.number of sectors read
    int num_msr;    //4.number of milliseconds spent reading
    int num_wcp;    //5.number of writes completed
    int num_mrw;    //6.number of writes merged Reads and writes which are adjacent to each other may be merged for efficiency
    int num_scw;    //7.number of sectors written
    int num_msw;    //8.number of milliseconds spent writing This is the total number of milliseconds spent by all writes
    int num_ioc;    //9.number of I/Os currently in progress.
    int num_cms;    //10.number of milliseconds spent doing I/Os.
    int num_wms;    //11.number of milliseconds spent doing I/Os, weighted.
    unsigned long long stp;  //time stamp of io_stat. unit: jiffy
};



//this is the struct from iostat command, dump it here for algorithm clarify
#if 0
struct io_stats {
    /* num of sectors read */
    unsigned long rd_sectors        __attribute__ ((aligned (8))); // 3. num_scr
    /* num of sectors written */
    unsigned long wr_sectors        __attribute__ ((packed));      // 7. num_scw
    /* num of read operations issued to the device */
    unsigned long rd_ios            __attribute__ ((packed));      // 1. num_ird
    /* num of read requests merged */
    unsigned long rd_merges         __attribute__ ((packed));      // 2. num_rdm
    /* num of write operations issued to the device */
    unsigned long wr_ios            __attribute__ ((packed));      // 5. num_wcp
    /* num of write requests merged */
    unsigned long wr_merges         __attribute__ ((packed));      // 6. num_mrw
    /* Time of read requests in queue */
    unsigned int  rd_ticks          __attribute__ ((packed));      // 4. num_msr
    /* Time of write requests in queue */
    unsigned int  wr_ticks          __attribute__ ((packed));      // 8. num_msw
    /* num of I/Os in progress */
    unsigned int  ios_pgr           __attribute__ ((packed));      // 9. num_ioc
    /* num of ticks total (for this device) for I/O */
    unsigned int  tot_ticks         __attribute__ ((packed));      //10. num_cms
    /* num of ticks requests spent in queue */
    unsigned int  rq_ticks          __attribute__ ((packed));      //11. num_wms
};
#endif                                                                                                                                                                                


struct io_stat blkst[2];

int get_emmc_stat(int i)
{
    int fd;
    char data[128];

    blkst[i].stp = get_uptime();
    
    fd = open(EMMC_DEV_STAT, O_RDONLY);
    if(fd < 0){
	printf("%s open failure\n", EMMC_DEV_STAT);
	return -1;
    }

    if(read(fd, data, 128) <= 0){
	printf("%s read failure\n", EMMC_DEV_STAT);
	close(fd);
	return -1;
    }
    sscanf(data, "%d %d %d %d %d %d %d %d %d %d %d", &blkst[i].num_ird,
	   &blkst[i].num_rdm, &blkst[i].num_scr, &blkst[i].num_msr, &blkst[i].num_wcp, &blkst[i].num_mrw,
	   &blkst[i].num_scw, &blkst[i].num_msw, &blkst[i].num_ioc, &blkst[i].num_cms, &blkst[i].num_wms);
    printf("%d %d %d %d %d %d %d %d %d %d %d\n", blkst[i].num_ird,
	   blkst[i].num_rdm, blkst[i].num_scr, blkst[i].num_msr, blkst[i].num_wcp, blkst[i].num_mrw,
	   blkst[i].num_scw, blkst[i].num_msw, blkst[i].num_ioc, blkst[i].num_cms, blkst[i].num_wms);
    
    close(fd);
    return 0;
}

#define HZ hz
unsigned long int hz = 0;
//get the number of ticks per second
void get_hz(void)
{
    long ticks;
    if ((ticks = sysconf(_SC_CLK_TCK)) == -1) {
	perror("sysconf");
    }
    hz = (unsigned int) ticks;
}
    

#define UPTIME "/proc/uptime"
//the return val is in unit of jiffies
unsigned long long get_uptime()
{
    int fd;
    char data[64];
    unsigned long long up_sec, up_csec;
    unsigned long long uptime;
    if(!hz){
	printf("HZ val not yet initialized\n");
	return -1;
    }
    
    fd = open(UPTIME, O_RDONLY);
    if(fd < 0){
	printf("open %s failure\n", UPTIME);
	return -1;
    }
    if(read(fd, data, 64) <= 0){
	printf("read %s failure\n", UPTIME);
	return -1;
    }
    sscanf(data, "%llu.%llu", &up_sec, &up_csec);
    uptime = up_sec*HZ + up_csec*HZ/100;
    
    printf("%s, up_sec:%lld, up_csec:%lld, uptime:%lld, HZ:%ld\n", data, up_sec, up_csec, uptime, HZ);
    close(fd);

    return uptime;
}





//return current io_stat index and update the current index
//to the next for the next time get_cur() called
static int cur = 0;
int get_cur()
{
    return cur;
}

void switch_cur(){
    cur = (cur+1)%2; 
}

#define LOADING_RD_THRES  200
#define LOADING_WR_THRES  200

#define FCTR 2048;
    

int get_io_loading()
{
    int i, j;
    double ldr, ldw;
    int interval;
    
    switch_cur();
    get_emmc_stat(get_cur());
    i = get_cur();
    j = (i+1)%2;
    if(!(blkst[i].stp - blkst[j].stp)){
	interval = 1;
    } else {
	interval = blkst[i].stp - blkst[j].stp;
    }

    printf("%d: %d,%d\n", i, blkst[i].num_scr, blkst[i].num_scw);
    printf("%d: %d,%d\n", j, blkst[j].num_scr, blkst[j].num_scw);

    printf("interval:%d\n", interval);
    
    ldr = (double)((blkst[i].num_scr - blkst[j].num_scr)*HZ)/interval/FCTR;
    ldw = (double)((blkst[i].num_scw - blkst[j].num_scw)*HZ)/interval/FCTR;

    printf("ldr:%8.2f, ldw:%8.2f\n", ldr, ldw);
    
    //TBD: current algorithm of deciding high loading is the be improved
    if(ldr > LOADING_RD_THRES || ldw > LOADING_WR_THRES) {
	return 1;   //loading high
    } else {
	return 0;   //loading normal
    }
}




int select_vm_data(int free)
{
    //TBD: select vm_data according to the free memory status
    //     and eMMC loading ==> not yet finished
    int io_loading_high;
    int thres;

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




int init()
{
    get_hz();
    get_emmc_stat(get_cur());
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
