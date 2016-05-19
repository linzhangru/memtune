#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ioloading.h"
#include <unistd.h>

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

