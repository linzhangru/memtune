#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/mman.h>
#include <malloc.h>

//struct timeval 
//{
//    time_t      tv_sec;     //seconds 
//    suseconds_t tv_usec;    //microseconds 
//};

int size[] = {
    //size for malloc chunk:
    16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 
    //size for mall tree chunk:
    256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288, 16384, 24576, 32768, 49152, 65536, 98304, 131072, 196608, 262144, 393216,
    524288, 786432, 1048576, 1572864, 2097152, 4194304, 6291456, 8388608, 12582912 
};

#define NUM_SIZES (sizeof(size)/sizeof(int))
#define NUM_THREADS 10

//10 threads, 12M/size*10 times of allocation for each thread each size
//use rand() to generate a random num eachtime time and check if chunks[thread_idx][size] exceeds 12M/size*10.
//if true, repeat, or else, record the val in the way of recording hte time interval

//the way of recording the time interval is as bellow:
//-----------------------------------------
//val(n+1) = {val(n)*n + x(n+1)}/(n+1)
//-----------------------------------------
//and val stands for the time interval


struct chunk_t {
    double val;
    int count;
};
struct chunk_t chunks[NUM_THREADS][NUM_SIZES];




#define ALLOC_SIZE  (1024*1024*1024UL)   //1G
#define ALLOC_TIMES (1024)

char * p[ALLOC_TIMES];

int main()
{

#if 0    
    int i;
    struct timeval  tv;
    struct timezone tz;
    long int t1, t2;
    long int size = ALLOC_SIZE/ALLOC_TIMES;
    //char * p;
    
    printf("size:%ld MB\n", (long int)(size/1024/1024.0));
    
    //mlockall(MCL_CURRENT|MCL_FUTURE);
    
    //TBD: malloc test: different size/times --> time accounting, wirte again and again
    //1G in total/1k each time
    //randomize the size of malloc, to preven the effect of free list in dlmalloc
    for(i = 0; i < ALLOC_TIMES; i+= 1){	
	gettimeofday(&tv, &tz);
	t1 = tv.tv_sec*1000000+tv.tv_usec;
	//printf("%8d: [%ld, %ld]\n", i, tv.tv_sec, tv.tv_usec);
	p[i] = malloc(size);
	mlock(p[i], size);
	memset(p[i], 0x5a5a5a5a, size);
	gettimeofday(&tv, &tz);
	t2 = tv.tv_sec*1000000+tv.tv_usec;
	//printf("%8d: [%ld, %ld]\n", i, tv.tv_sec, tv.tv_usec);
	printf("%ld, ", t2-t1);
    }

    //munlockall();
    for(i = 0; i < ALLOC_TIMES; i+= 1){
	munlock(p[i], size);
	free(p[i]);
	if(!malloc_trim(size)){
	    //printf("\nmalloc_trim fail!");
	}
    }

#else
    int val;
    int i;
    for(i = 0; i < 10; i++){
	val = rand();
	printf("rand: %d\n", val);
    }
#endif

    
    
    //TBD: mmap test --> time accounting, mmap write munmap again and again


    //TBD: file read/write --> time accounting, file file read/write again and again
    
}

