#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;
    char * p;
    
    //TBD: malloc test: different size/times --> time accounting, wirte again and again
    //500M in total/1k each time
    for(i = 0; i < 500*1024; i+= 1){	
	t1 = gettimeofday();
	p = malloc(1024);
	memset(p, 0x5a5a5a5a, 1024/4);
	t2 = gettimeofday();
    }
    
    
    //TBD: mmap test


    //TBD: file read/write
    
}

