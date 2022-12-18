#include <stdio.h> 
#include "fifo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main(){
    int num_writer = 1; 
    int num_readers = 1;
    struct fifo *fifo_pipe = mmap(NULL, sizeof(struct fifo), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    fifo_init(fifo_pipe);
    for(int i = 1; i <= num_writer; i++){
        int c; 
        if(c = fork() == 0){
            // Do Write 
            for(unsigned long d = (i-1)*1000; d < (i*1000 - 1); d++) {
                fprintf(stderr, "Printing number #pnum %d, digit %d.  \n", i, d);
                fifo_wr(fifo_pipe, d);
            }
            exit(0);
        }
        
    }
    
  //  for(int i = 0; i < 1000; i++){
   //     unsigned long d = fifo_rd(fifo_pipe);
   //     fprintf(stderr, "%d \n", d);
   // }

    wait(NULL);
    exit(0);


        
    }
