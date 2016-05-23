#include <stdio.h>

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
    char data[5*NUM_CHUNK_TYPE+1];
    int i, j;
    int count = 0;
    int val;
    
    
    
    if(argc != 2){
	printf("Usage:\n"
	       "rsltesti <logfile_name>\n");
	return -1;
    }
    fd = open(arg[0], O_RDONLY);
    if(fd < 0){
	printf("fail to open file %s\n", arg[0]);
	return -2;
    }

    
    memset(data, 0x0, NUM_CHUNK_TYPE);
    if(read(fd, data, NUM_CHUNK_TYPE) != NUM_CHUNK_YTPE){
	printf("faile to read file %s\n");
	return -3;
    }
    for(i = 0; i < NUM_CHUNK_TYPE; i++){
	sscanf(data+6*i, "%5.2f,",  &val);
	records[i].exp = val;
	records[i].max = val;
	records[i].min = val;
    }
    
    
    while(read(fd, data, NUM_CHUNK_TYPE) == NUM_CHUNK_YTPE){
	for(i = 0; i < NUM_CHUNK_TYPE; i++){
	    sscanf(data+6*i, "%5.2f,",  &val);
	    records[i].exp = (records[i].exp + val)/2;	    
	    if(val > records[i].max) {
		records[i].max = val;
	    }
	    if(val < records[i].min) {
		records[i].min = val;
	    }
	}
	memset(data, 0x0, NUM_CHUNK_TYPE);
    }
    
    close(fd);
}
