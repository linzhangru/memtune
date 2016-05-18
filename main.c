#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>




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

    

struct entry meminfo[SIZE_MEMINFO] = {
    {"MemTotal:",          INVALID_VAL},
    {"MemFree:",           INVALID_VAL},
    {"MemAvailable:",      INVALID_VAL},
    {"Buffers:",           INVALID_VAL},
    {"Cached:",            INVALID_VAL},
    {"SwapCached:",        INVALID_VAL},
    {"Active:",            INVALID_VAL},
    {"Inactive:",          INVALID_VAL},
    {"Active(anon):",      INVALID_VAL},
    {"Inactive(anon):",    INVALID_VAL},
    {"Active(file):",      INVALID_VAL},
    {"Inactive(file):",    INVALID_VAL},
    {"Unevictable:",       INVALID_VAL},
    {"Mlocked:",           INVALID_VAL},
    {"SwapTotal:",         INVALID_VAL},
    {"SwapFree:",          INVALID_VAL},
    {"Dirty:",             INVALID_VAL},
    {"Writeback:",         INVALID_VAL},
    {"AnonPages:",         INVALID_VAL},
    {"Mapped:",            INVALID_VAL},
    {"Shmem:",             INVALID_VAL},
    {"Slab:",              INVALID_VAL},
    {"SReclaimable:",      INVALID_VAL},
    {"SUnreclaim:",        INVALID_VAL},
    {"KernelStack:",       INVALID_VAL},
    {"PageTables:",        INVALID_VAL},
    {"NFS_Unstable:",      INVALID_VAL},
    {"Bounce:",            INVALID_VAL},
    {"WritebackTmp:",      INVALID_VAL},
    {"CommitLimit:",       INVALID_VAL},
    {"Committed_AS:",      INVALID_VAL},
    {"VmallocTotal:",      INVALID_VAL},
    {"VmallocUsed:",       INVALID_VAL},
    {"VmallocChunk:",      INVALID_VAL},
    {"HardwareCorrupted:", INVALID_VAL},
    {"AnonHugePages:",     INVALID_VAL},
    {"CmaTotal:",          INVALID_VAL},
    {"CmaFree:",           INVALID_VAL},
    {"HugePages_Total:",   INVALID_VAL},
    {"HugePages_Free:",    INVALID_VAL},
    {"HugePages_Rsvd:",    INVALID_VAL},
    {"HugePages_Surp:",    INVALID_VAL},
    {"Hugepagesize:",      INVALID_VAL},
    {"DirectMap4k:",       INVALID_VAL},
    {"DirectMap2M:",       INVALID_VAL}
};

		      
const int NUM_ENTRY = sizeof(meminfo)/sizeof(struct entry);
		      
int parse_meminfo()
{
    char data[4096];
    char * pline;
    char tmp[32];
    int szfree, szbuffer, szcache;
    int val;
    
    int i;
    
    memset(data, 0, 4096);
    memset(tmp, 0, 32);

    int fd = open("/proc/meminfo",O_RDONLY);
    if(fd < 0){
	printf("fail to open /proc/meminfo!\n");
	return -1;
    }
    //printf("fd:%d\n", fd);
    if(read(fd, data, 4096) <= 0){
	printf("/proc/meminfo read fail\n");
	return -1;
    }
    

    for(i = 0; i < NUM_ENTRY; i++){
	char tmp[64] = {0};
	pline = NULL;
	//pline = strstr(data, "MemFree:");
	pline = strstr(data, meminfo[i].name);
	if(pline == NULL){
	    printf("fail to parse MemFree\n");
	    return -1;
	}
	sscanf(pline+strlen(meminfo[i].name), "%ld kB.*", &meminfo[i].val);
	//printf("%s %ld\n", meminfo[i].name, meminfo[i].val);
    }

    //printf("%s\n", data);

    close(fd);
}

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
struct block_stat {
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


struct block_stat blkst;

int get_emmc_stat()
{
    int fd;
    char data[128];
    
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
    sscanf(data, "%d %d %d %d %d %d %d %d %d %d %d", &blkst.num_ird,
	   &blkst.num_rdm, &blkst.num_scr, &blkst.num_msr, &blkst.num_wcp, &blkst.num_mrw,
	   &blkst.num_scw, &blkst.num_msw, &blkst.num_ioc, &blkst.num_cms, &blkst.num_wms);
    printf("%d %d %d %d %d %d %d %d %d %d %d\n", blkst.num_ird,
	   blkst.num_rdm, blkst.num_scr, blkst.num_msr, blkst.num_wcp, blkst.num_mrw,
	   blkst.num_scw, blkst.num_msw, blkst.num_ioc, blkst.num_cms, blkst.num_wms);
    
    close(fd);
    return 0;
}


int select_vm_data(int free)
{
    //TBD: select vm_data according to the free memory status
    //     and eMMC loading ==> not yet finished

    get_emmc_stat();
    
    int thres;
    if(free/1024 > THRESHOLD_0)
	return 0;
    else if(free/1024 > THRESHOLD_1)
	return 1;
    else if(free/1024 > THRESHOLD_2)
	return 2;
    else
	return 0;  //TBD: consider about more vm configration choices
}




int main()
{
    unsigned long free;
    int choice;
    int sec = 0;

    //get_emmc_stat();
    
#if 1   
    while(1){
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
