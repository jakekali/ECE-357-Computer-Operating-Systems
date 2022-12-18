#include <stdio.h> 
#include "fifo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#define NUM_WRITERS 1
#define NUM_INTERS 10000
int counter[NUM_WRITERS]; 

int main(){
    int num_writer = NUM_WRITERS; 
    int num_readers = 1;
    struct fifo *fifo_pipe = mmap(NULL, sizeof(struct fifo), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if(fifo_pipe == 0) {
        fprintf(stderr, "Error: %s \n", strerror(errno));
    }
    fifo_init(fifo_pipe);
    for(int i = 1; i <= num_writer; i++){
        int c; 
        if(c = fork() == 0){
            // Do Write 
            for(unsigned long d = (i-1)*NUM_INTERS; d <= (i*NUM_INTERS - 1); d++) {
           fprintf(stderr, "Printing number #pnum %d, digit %d.  \n", i, d);
                fifo_wr(fifo_pipe, d);
            }
    
        //   kill(c-1, SIGUSR1);
            exit(0);
        }
        
    }
    

    for(int i = 1; i <= NUM_INTERS* num_writer; i++){
        unsigned long d = fifo_rd(fifo_pipe);
        int index = d / (NUM_INTERS);
        if(counter[index] > d){
            fprintf(stderr, "HOLY SHIT WE HAVE A PROBLEM d = %d \n", d);
            fprintf(stderr, "Max in index %d, is %d \n", index, counter[index]);

        }
        fprintf(stderr, "%d \n", d);
        counter[index] = d;
    }

    for(int i = 0; i < num_writer; i++) {
        fprintf(stderr, "Max in index %d, is %d \n", i, counter[i]);
    }

    for(int i = 0; i < num_writer + 1; i++) {
        wait(NULL);
    }
    exit(0);


        
    }
