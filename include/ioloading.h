#ifndef __IOLOADING__
#define __IOLOADING__

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

extern unsigned long long get_uptime();
extern int get_io_loading();
extern void iostat_init();


#endif
