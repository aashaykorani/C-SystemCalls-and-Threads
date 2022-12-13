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
    if(argc<5){
        printf("\nUsage: ./run <filename> <-r|-w> <block_size> <block_count>\n");
        exit(1);
    }
    if(strcmp(argv[2],"-r")!=0 && strcmp(argv[2],"-w")!=0){
        printf("\nUsage: ./run <filename> <-r|-w> <block_size> <block_count>\n");
        exit(1);
    }
    if(atoi(argv[3])<=0){
        printf("\nBlock size cannot be a string or an integer <= 0\n");
        exit(1);
    }
    if(atoi(argv[4])<=0){
        printf("\nBlock count cannot be a string or an integer <= 0\n");
        exit(1);
}
}

void reader(char * argv[]){

    int block_size = atoi(argv[3]);
    int block_count = atoi(argv[4]);
    unsigned int result = 0;
    int bytes_read;
    double total_bytes_read = 0;
    int buffer[block_size];
    struct timespec spec,endspec;



    int fd = open(argv[1],O_RDONLY);
    if(fd==-1){
        printf("Error: %s\n",strerror(errno));
        exit(1);
    }

    clock_gettime(CLOCK_REALTIME,&spec);

    while(block_count != 0 && ((bytes_read = read(fd,buffer,block_size)) > 0)){
        result ^= xorbuf(buffer,ceil(bytes_read/4));
        block_count -= 1;
        total_bytes_read += bytes_read;
    }

    clock_gettime(CLOCK_REALTIME,&endspec);
    close(fd);

    long seconds = endspec.tv_sec - spec.tv_sec;
    long nanoseconds = endspec.tv_nsec - spec.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    double speed = (total_bytes_read/1048576.0)/elapsed;
    
    printf("Read %.f bytes in %fs\n",total_bytes_read,elapsed);
    printf("Reading Speed = %.2f MB/s\n",speed);
    printf("XOR %08x\n",result);
}

void writer(char * argv[]){

    int block_size = atoi(argv[3]);
    int block_count = atoi(argv[4]);
    int bytes_read,bytes_written;
    double total_bytes_written = 0;
    int buffer[block_size];
    struct timespec spec,endspec;


    int fd2 = open(argv[1],O_CREAT|O_WRONLY,00644);
    if(fd2==-1){
        printf("Error: %s\n",strerror(errno));
        exit(1);
    }

    clock_gettime(CLOCK_REALTIME,&spec);
    
    while(block_count != 0){
        bytes_written = write(fd2,buffer,block_size);
        block_count -= 1;
        total_bytes_written += bytes_written;
    }

    
    clock_gettime(CLOCK_REALTIME,&endspec);
    close(fd2);

    long seconds = endspec.tv_sec - spec.tv_sec;
    long nanoseconds = endspec.tv_nsec - spec.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    double speed = (total_bytes_written/1048576.0)/elapsed;

    
    printf("Wrote %.f bytes in %fs to file named \'%s\'\n",total_bytes_written,elapsed,argv[1]);
    printf("Writing Speed = %.2f MB/s\n",speed);

}

int main(int argc, char* argv[]){

    checker(argc, argv);

    if(strcmp(argv[2],"-r")==0)
        reader(argv);
    else
        writer(argv);


    return 0;
}