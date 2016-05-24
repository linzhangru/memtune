#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define NUM_CHUNK_TYPE 61

struct entry {
    double exp;
    double max;
    double min;
};

struct entry records[NUM_CHUNK_TYPE];

    

    
int main(int argc, char* argv[])
{
    int fd;
    char data[6*NUM_CHUNK_TYPE+2];
    int i, j;
    int count = 0;
    float val;
    
    
    if(argc != 2){
	printf("Usage:\n"
	       "rsltesti <logfile_name>\n");
	return -1;
    }
    fd = open(argv[1], O_RDONLY);
    if(fd < 0){
	printf("fail to open file %s\n", argv[1]);
	return -2;
    }


    printf("reading line %d:\n", count++);
    memset(data, 0x0, NUM_CHUNK_TYPE*6+2);
    if(read(fd, data, NUM_CHUNK_TYPE*6+1) != NUM_CHUNK_TYPE*6+1){
	printf("faile to read file %s\n", argv[1]);
	return -3;
    }
    for(i = 0; i < NUM_CHUNK_TYPE; i++){
	sscanf(data+6*i, "%f,",  &val);
	//printf("%5.2f,", val);
	records[i].exp = val;
	records[i].max = val;
	records[i].min = val;
    }
    //printf("%5.2f\n", val);
    
    while(read(fd, data, NUM_CHUNK_TYPE*6+1) == NUM_CHUNK_TYPE*6+1){
	printf("reading line %d:\n", count++);
	for(i = 0; i < NUM_CHUNK_TYPE; i++){
	    sscanf(data+6*i, "%f,",  &val);
	    //printf("%5.2f,", val);
	    records[i].exp = (records[i].exp + val)/2;	    
	    if(val > records[i].max) {
		records[i].max = val;
	    }
	    if(val < records[i].min) {
		records[i].min = val;
	    }
	}
	//printf("%5.2f\n", val);
	memset(data, 0x0, NUM_CHUNK_TYPE);
    }
    
    close(fd);


    printf("\nexpectation:\n");
    for(i = 0; i < NUM_CHUNK_TYPE; i++){
	printf("%5.2f,", records[i].exp);
    }
    printf("\nmax:\n");
    for(i = 0; i < NUM_CHUNK_TYPE; i++){
	printf("%5.2f,", records[i].max);
    }
    printf("\nmin\n");
    for(i = 0; i < NUM_CHUNK_TYPE; i++){
	printf("%5.2f,", records[i].min);
    }
    return 0;
}
