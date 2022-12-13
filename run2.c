#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <errno.h>

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void checker(int argc, char* argv[]){

    if(argc<3 || argc>4){
        printf("\nUsage: ./run2 <file_name> <block_count> [optional:<time_to_run> (default 6s)]\n");
        printf("Example: ./run2 input.txt 50\nExample: ./run2 input.txt 50 9\n");
        exit(1);
    }
    if(atoi(argv[2])<=0){
        printf("\nBlock size cannot be a string or an integer <= 0\n");
        exit(1);
    }
    if(argv[3]!=NULL && (atoi(argv[3])<5 || atoi(argv[3])>15)){
        printf("\nReasonable time is between 5 to 15 seconds.\n");
        exit(1);
    }
}

void reader(char * argv[]){

    int block_size = atoi(argv[2]);
    int block_count = 0;
    int bytes_read;
    unsigned int result = 0;
    double total_bytes_read = 0;
    int buffer[block_size];
    float rtime = 6.0;
    if(argv[3]!=NULL)
        rtime = atoi(argv[3]);
    else
        printf("No time arg supplied. Defaulting to %.fs.\n",rtime);
    clock_t start;



    int fd = open(argv[1],O_RDONLY);
    if(fd==-1){
        // printf("This program needs the file \'ubuntu-21.04-desktop-amd64.iso\' in the current directory.\n");
        printf("Error: %s\n",strerror(errno));
        exit(1);
    }
    
    float time_taken;

    start = clock();
    while((time_taken = (float)(clock() - start) / CLOCKS_PER_SEC) < rtime && ((bytes_read = read(fd,buffer,block_size)) > 0)){
        result ^= xorbuf(buffer,ceil(bytes_read/4));
        block_count += 1;
        total_bytes_read += bytes_read;

    }
    close(fd);
    if(time_taken<rtime){
        printf("\nEntire file has been read\n");
        printf("\nBlock count = %d\nTime taken = %f seconds.\n",block_count,time_taken);
        printf("Reading Speed = %.2f MB/s\n",(total_bytes_read/1048576.0)/time_taken);
        printf("XOR %08x\n",result);
    }
    else{
        printf("\nFile size read = %.f bytes\nBlock count = %d\nTime taken = %f seconds.\n",total_bytes_read,block_count,time_taken);
        printf("Reading Speed = %.2f MB/s\n",(total_bytes_read/1048576.0)/time_taken);
        printf("XOR %08x\n",result);
    }
}


int main(int argc, char* argv[]){

    checker(argc, argv);
    reader(argv);

    return 0;
}