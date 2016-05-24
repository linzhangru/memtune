#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "meminfo.h"

#if defined(_X86_)
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
#elif defined(_AARCH64_)
struct entry meminfo[SIZE_MEMINFO] = {
    {"MemTotal:",          INVALID_VAL},
    {"MemFree:",           INVALID_VAL},
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
    {"VmallocAlloc:",      INVALID_VAL},
    {"VmallocChunk:",      INVALID_VAL},
    {"IonTotal:",          INVALID_VAL},
    {"IonInUse:",          INVALID_VAL},
    {"SubTotal:",          INVALID_VAL}
};
#else
#error "not supportted architecture"
#endif


		      
const int NUM_ENTRY = sizeof(meminfo)/sizeof(struct entry);
		      
int parse_meminfo()
{
    char data[4096];
    char * pline;
    char tmp[32];
    
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
	pline = NULL;
	pline = strstr(data, meminfo[i].name);
	if(pline == NULL){
	    printf("fail to parse MemFree\n");
	    return -1;
	}
	sscanf(pline+strlen(meminfo[i].name), "%ld kB.*", &meminfo[i].val);
	printf("%s %ld\n", meminfo[i].name, meminfo[i].val);
    }

    //printf("%s\n", data);

    close(fd);
    return 0;
}
