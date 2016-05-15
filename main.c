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



struct entry vmfds[NR_FDS] = {
    {"/proc/sys/vm/dirty_expire_centisecs",    -1},
    {"/proc/sys/vm/dirty_writeback_centisecs", -1},
    {"/proc/sys/vm/dirty_ratio",               -1},
    {"/proc/sys/vm/dirty_background_ratio",   -1},
    {"/proc/sys/vm/min_free_kbytes",           -1},
    {"/proc/sys/vm/vfs_cache_pressure",        -1},
    {"/proc/sys/vm/swappiness",                -1}
};



int selet_vm_data()
{
    //TBD: select vm_data according to the free memory status
}


int get_vm_data(struct entry vmfds[])
{
    char data[64];
    unsigned long val;

    for(int i; i < NR_FDS; i++){
	vmfds[i].val = open(vmfds[i].name, O_RDWR);
	if(vmfds[i].val < 0){
	    printf("fail to open file %s: %ld\n", vmfds[i].name, vmfds[i].val);
	    return -1;
	}
	//printf("%s:%ld\n", vmfds[i].name, vmfds[i].val);
    }
    printf("all vm files open succeeded\n");

    
    for(int i; i < NR_FDS; i++){
	memset(data, 0, 64);
	//fscanf(vmfds[i].val, "%d", &val);
	if(read(vmfds[i].val, data, 64) <= 0){
	    printf("file %s reads failure\n", vmfds[i].name);
	    return -1;
	}
	sscanf(data, "%ld", &val);
	printf("_%s: %ld\n", vmfds[i].name, val);
    }

    for(int i; i < NR_FDS; i++){    
	if(vmfds[i].val >= 0)
	    close(vmfds[i].val);
    }


}



int reconfig_vmfds()
{
    //TBD: inject new config datas through sysctl.vm
}


int main()
{
    unsigned long free;
    
    while(1){
	parse_meminfo();
	free = meminfo[MEMFREE].val + meminfo[CACHED].val;
	printf("free: %ld(F) + %ld(C) = %ld MB\n", meminfo[MEMFREE].val/1024, meminfo[CACHED].val/1024, free/1024);
	get_vm_data(vmfds);
	sleep(1);
    }
    return 0;
}
