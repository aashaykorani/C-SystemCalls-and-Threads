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

    if(argc<2 || argc>3){
        printf("\nUsage: ./run2 <block_count> [optional:<time_to_run> (default 6s)]\n");
        printf("Example: ./run2 50\nExample: ./run2 50 6\n");
        exit(1);
    }
    if(atoi(argv[1])<=0){
        printf("\nBlock size cannot be a string or an integer <= 0\n");
        exit(1);
    }
    if(argv[2]!=NULL && (atoi(argv[2])<5 || atoi(argv[2])>15)){
        printf("\nReasonable time is between 5 to 15 seconds.\n");
        exit(1);
    }
}

void reader(char * argv[]){

    int block_size = atoi(argv[1]);
    int block_count = 0;
    int bytes_read;
    double total_bytes_read = 0;
    int buffer[block_size];
    float rtime = 6.0;
    if(argv[2]!=NULL)
        rtime = atoi(argv[2]);
    else
        printf("No time arg supplied. Defaulting to %.fs.\n",rtime);
    clock_t start;



    int fd = open("ubuntu-21.04-desktop-amd64.iso",O_RDONLY);
    if(fd==-1){
        printf("This program needs the file \'ubuntu-21.04-desktop-amd64.iso\' in the current directory.\n");
        printf("%s\n",strerror(errno));
        exit(1);
    }
    
    float time_taken;

    start = clock();
    again:while((time_taken = (float)(clock() - start) / CLOCKS_PER_SEC) < rtime && ((bytes_read = read(fd,buffer,block_size)) > 0)){
        
        block_count += 1;
        total_bytes_read += bytes_read;

    }
    if(time_taken<rtime){
        lseek(fd,0,SEEK_SET);
        goto again;
    }

    printf("File size read = %.f bytes\nBlock size = %d\nTime taken = %f seconds.\n",total_bytes_read,atoi(argv[1]),time_taken);
    printf("No. of read system calls made = %.2f\n",(total_bytes_read)/time_taken);
    close(fd);

}

void seeker(char * argv[]){

    int block_size = atoi(argv[1]);
    int block_count = 0;
    int bytes_read;
    double total_bytes_read = 0;
    int buffer[block_size];
    float rtime = 6.0;
    if(argv[2]!=NULL)
        rtime = atoi(argv[2]);
    else
        printf("No time arg supplied. Defaulting to %.fs.\n",rtime);
    clock_t start;



    int fd = open("ubuntu-21.04-desktop-amd64.iso",O_RDONLY);
    if(fd==-1){
        printf("This program needs the file \'ubuntu-21.04-desktop-amd64.iso\' in the current directory.\n");
        printf("%s\n",strerror(errno));
        exit(1);
    }
    
    float time_taken;

    start = clock();
    again:while((time_taken = (float)(clock() - start) / CLOCKS_PER_SEC) < rtime && ((bytes_read = lseek(fd,1,SEEK_CUR)) > 0)){
        
        block_count += 1;
        total_bytes_read += bytes_read;

    }
    if(time_taken<rtime){
        lseek(fd,0,SEEK_SET);
        goto again;
    }

    printf("File size seeked = %.f bytes\nBlock size = %d\nTime taken = %f seconds.\n",total_bytes_read,atoi(argv[1]),time_taken);
    printf("No. of read lseek system calls made = %.2f\n",(total_bytes_read)/time_taken);
    close(fd);

}


int main(int argc, char* argv[]){

    // checker(argc, argv);
    if(strcmp(argv[3],"-r")==0)
        reader(argv);
    else
        seeker(argv);

    return 0;
}