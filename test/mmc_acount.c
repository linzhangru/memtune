#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/mman.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

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

#define NUM_CHUNK_TYPE (sizeof(size)/sizeof(int))
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
struct chunk_t chunks[NUM_THREADS][NUM_CHUNK_TYPE];








void seeding()
{
    struct timeval  tv;
    struct timezone tz;
    unsigned int t;

    gettimeofday(&tv, &tz);
    t = tv.tv_sec*1000000 + tv.tv_usec;
    srand(t);
}



#define ALLOC_SIZE  (1024*1024*1024UL)   //1G
#define ALLOC_TIMES (1024)

char * p[ALLOC_TIMES];


int get_size_index()
{
    seeding();
    //The rand() function returns a pseudo-random integer in the range 0 to RAND_MAX inclusive
    //(i.e., the mathematical range [0, RAND_MAX]).
    //The srand() function returns no value.
    //use "man 3 rand" to gather the info of function rand()
    return rand()%NUM_CHUNK_TYPE;
}

int free_or_not()
{
    seeding();
    return rand()%2;
}


int main()
{
    int szidx;
    int i;
    struct timeval  tv;
    struct timezone tz;
    long int t1, t2;
    char * p;

    for(i = 0; i < NUM_CHUNK_TYPE; i++) {
	chunks[0][i].val = 0.0;
	chunks[0][i].count = 0;
    }
    
    
    //printf("RAND_MAX:%d, NUM_CHUNK_TYPE:%ld\n", RAND_MAX, NUM_CHUNK_TYPE);
    //when i reaches 36244, the system will become stuck,
    //so we set the top val as 20480
    for(i = 0; i < 16*1024; i++){
	//we abandon the malloced memory section directly
	//and it will be freed by OS after process exits.
	p = NULL;
	if(!(i%256)){
	    //printf("%d\n",i);
	}
	
	szidx = get_size_index();
	//printf("szidx: %d\n", szidx);

	gettimeofday(&tv, &tz);
	t1 = tv.tv_sec*1000000+tv.tv_usec;
	
	p = malloc(size[szidx]);
	if(!p) {
	    printf("malloc failure, size:%d\n", size[szidx]);
	    continue;
	}
	
	
	memset(p, 0x5a5a5a5a, size[szidx]);
	
	gettimeofday(&tv, &tz);
	t2 = tv.tv_sec*1000000+tv.tv_usec;

	//val(n+1) = {val(n)*n + x(n+1)}/(n+1)
	if(!chunks[0][szidx].count){
	    chunks[0][szidx].val = t2 - t1;
	} else {
	    chunks[0][szidx].val = (chunks[0][szidx].val*chunks[0][szidx].count + (t2-t1))/chunks[0][szidx].count;
	}
	chunks[0][szidx].count++;
	//area decided to be freed
	if(free_or_not() && !!p){
	    free(p);
	}
    }

    for(i = 0; i < NUM_CHUNK_TYPE; i++) {
	    printf(/*"size:%8d, time:"*/"%.2f,"
		   /*"count:%d\n", size[i]*/,log(100*chunks[0][i].val)/*, chunks[0][i].count*/);
    }

    
    
    //TBD: mmap test --> time accounting, mmap write munmap again and again


    //TBD: file read/write --> time accounting, file file read/write again and again
    return 0;
}

