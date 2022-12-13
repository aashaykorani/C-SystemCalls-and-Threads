#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>


int block_count = 28187, number_of_threads = 30, blocks_per_thread;
int balance, block_size = 100000;
float f_block_per_thread, total_file_size;
double global_bytes_read = 0;
char *file_name;
int file_pointer[15];


unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void checker(int argc, char* argv[]){

    if(argc<2){
        printf("Usage: ./threaded <filename> [optional: <block_size> (default: 100000) <block_count> (default: 10^9) <number_of_threads> (default: 30)]\n");
        printf("Example: ./threaded input.txt\nExample: ./threaded input.txt 50 200\nExample: ./threaded input.txt 50 200 12\n");
        exit(1);
    }

    if(argc>=2 && argv[2]!=NULL){
        if(atoi(argv[2])<=0){
            printf("Block size cannot be a string or an integer <= 0\n");
            exit(1);
        }
        else
            block_size = atoi(argv[2]);
        
    }
    if(argc>=3 && argv[3]!=NULL){
        if(atoi(argv[3])<=0){
            printf("arg %s\n",argv[3]);
            printf("Block count cannot be a string or an integer <= 0\n");
            exit(1);
        }
        else
            block_count = atoi(argv[3]);
    }
    if(argc>=4 && argv[4]!=NULL){
        if(atoi(argv[4])<=0){
            printf("Number of threads cannot be a string or an integer <= 0\n");
            exit(1);
        }
        else
            number_of_threads = atoi(argv[4]);
    }
}


void * reader(void * arg){

    
    long tid = (long)arg;
    int bytes_read;
    double total_bytes_read = 0;
    int buffer[block_size];
    int individual_count;
    unsigned int result = 0;
    
    if(tid==number_of_threads-1)
        individual_count = balance+blocks_per_thread;
    else
        individual_count = blocks_per_thread;

    while(individual_count!=0 && (bytes_read = read(file_pointer[tid],buffer,block_size))>0){
        
        result ^= xorbuf(buffer,ceil(bytes_read/4));
        individual_count-=1;
        total_bytes_read += bytes_read;
    }

    global_bytes_read += total_bytes_read;
    
    printf("[Thread %ld read]: %.f bytes\n",tid,total_bytes_read);
    close(file_pointer[tid]);
    
    pthread_exit((void *)result);

}

int main(int argc, char * argv[]){

    checker(argc,argv);
    
    pthread_t *fr;
    long i;
    file_name = argv[1];
    
    
    printf("Block size = %d\n",block_size);
    printf("Block count = %d\n",block_count);
    printf("No. of threads = 30.\n");

    // Get the total file size

    int fp;
    if((fp = open(file_name,O_RDONLY))<0){
        printf("Error: %s\n",strerror(errno));
        exit(1);
    }
    total_file_size = lseek(fp,0,SEEK_END);
    close(fp);
    
    /*
    If: the block_count*block_size > file_size
    Count the number of blocks it will take to read the entire file with the specified block_size.
    Then divide the blocks between all the threads.

    Else: Divide the block count between the threads

    In case of unequal division, the left out blocks are assigned to the last thread.
    */

    if(block_count*(float)block_size < total_file_size){
        f_block_per_thread = ((float)block_count/(float)number_of_threads);
        blocks_per_thread = floor(f_block_per_thread);
    }
    else{
        float inter = (total_file_size/(float)block_size);
        f_block_per_thread = inter/(float)number_of_threads;
        blocks_per_thread = ceil(f_block_per_thread);
    }

    printf("Avg blocks_per_thread = %d\n",blocks_per_thread);
    
    float sub = fabs(f_block_per_thread-blocks_per_thread);
    balance = (int)round(sub*(float)number_of_threads);
    
    fr = (pthread_t *) malloc(sizeof(pthread_t)*number_of_threads);

    // Every thread will read blocks_per_thread*block_size bytes of data.
    
    for(i=0;i<number_of_threads;i++){
        fp = open(file_name,O_RDONLY);
        lseek(fp,(i*blocks_per_thread*block_size),SEEK_SET);
        file_pointer[i] = fp;
    }

    struct timespec spec,endspec;
    clock_gettime(CLOCK_REALTIME,&spec);

    for(i=0;i<number_of_threads;i++){
        pthread_create(&fr[i],NULL,reader,(void*)i);
        if(i==number_of_threads-1)
            block_count-=(balance+blocks_per_thread);
        else
            block_count-=(int)(blocks_per_thread);
    }
    
    void * result = NULL;
    unsigned int ret = 0;
    
    for(i=0;i<number_of_threads;i++){
        pthread_join(fr[i],&result);
        ret ^= (unsigned int)result;
    }
    clock_gettime(CLOCK_REALTIME,&endspec);
    

    long seconds = endspec.tv_sec - spec.tv_sec;
    long nanoseconds = endspec.tv_nsec - spec.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    printf("\nTime = %f\n",elapsed);
    printf("Total bytes read = %f\n",global_bytes_read);
    printf("Speed = %.2f MB/s\n",(global_bytes_read/1048576.0)/elapsed);
    printf("XOR %08x\n",ret);

    return 0;
}           