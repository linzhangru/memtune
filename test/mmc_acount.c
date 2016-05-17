#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/mman.h>


//struct timeval 
//{
//    time_t      tv_sec;     //seconds 
//    suseconds_t tv_usec;    //microseconds 
//};


#define ALLOC_SIZE  1024*1024*1024   //1G
#define ALLOC_TIMES 32

int main()
{
    int i;
    struct timeval  tv;
    struct timezone tz;
    long int t1, t2;
    int size = ALLOC_SIZE/ALLOC_TIMES;
    //char * p;
    char * p[ALLOC_TIMES];
    
    //mlockall(MCL_CURRENT|MCL_FUTURE);
    
    //TBD: malloc test: different size/times --> time accounting, wirte again and again
    //1G in total/1k each time
    //randomize the size of malloc, to preven the effect of free list in dlmalloc
    for(i = 0; i < ALLOC_TIMES; i+= 1){	
	gettimeofday(&tv, &tz);
	t1 = tv.tv_sec*1000000+tv.tv_usec;
	//printf("%8d: [%ld, %ld]\n", i, tv.tv_sec, tv.tv_usec);
	p[i] = malloc(size);
	memset(p[i], 0x5a5a5a5a, size);
	mlock(p[i], size);
	gettimeofday(&tv, &tz);
	t2 = tv.tv_sec*1000000+tv.tv_usec;
	//printf("%8d: [%ld, %ld]\n", i, tv.tv_sec, tv.tv_usec);
	printf("%ld, ", t2-t1);
    }

    //munlockall();
    for(i = 0; i < ALLOC_TIMES; i+= 1){
	munlock(p[i], size);
	free(p[i]);
    }
    
    
    //TBD: mmap test --> time accounting, mmap write munmap again and again


    //TBD: file read/write --> time accounting, file file read/write again and again
    
}

