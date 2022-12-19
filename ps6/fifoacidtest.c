#include <stdio.h> 
#include "fifo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>


int counter[64]; 

int main(int  argc, char **argv){
    int num_writer = atoi(argv[1]); 
    int num_readers = 1;
    int  numofit = atoi(argv[2]);
    struct fifo *fifo_pipe = mmap(NULL, sizeof(struct fifo), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if(fifo_pipe == 0) {
        fprintf(stderr, "Error: %s \n", strerror(errno));
    }

    fifo_init(fifo_pipe);
    for(int i = 1; i <= num_writer; i++){
        int c; 
        if(c = fork() == 0){
            // Do Write 
            for(unsigned long d = (i-1)*numofit; d <= (i*numofit - 1); d++) {
      //     fprintf(stderr, "Printing number #pnum %d, digit %d.  \n", i, d);
                fifo_wr(fifo_pipe, d);
            }
    
        
            exit(0);
        }
        
    }
    

    for(int i = 1; i <= numofit* num_writer; i++){
        unsigned long d = fifo_rd(fifo_pipe);
        int index = d / (numofit);
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

    for(int i = 0; i < num_writer; i++) {
        wait(NULL);
    }
    exit(0);


        
    }
