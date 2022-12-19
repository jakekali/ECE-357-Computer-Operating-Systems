#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "spinlock.h"
#include <errno.h>
int main(int argc, char **argv) {
	int *lockcount = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	int  *nolockcount = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	struct spinlock *l = mmap(NULL, sizeof(struct spinlock), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	if((l || lockcount || nolockcount) == -1) {
		perror("Error with mmap");
		
	}
	l->lock = 0;
	pid_t child_pid;
	if(argc != 3) {
		fprintf(stderr, "Usage <nchild>  <num of iterations>\n");
		exit(-1);

	}
	int numofchild = atoi(argv[1]);
	int numofit = atoi(argv[2]);
	for (int i = 0; i < numofchild; i++) {
		
		if((child_pid = fork())== 0) {
				
	for(int c = 0; c < numofit; c++) {
		spin_lock(l);
		(*lockcount)++;
			spin_unlock(l);

			(*nolockcount)++;
		}
		
	
		
	
	
			
exit(0);

	}	
 
}
for (int i = 0; i < numofchild; i++) {
	wait(NULL);
}
	for (int i = 0; i < numofchild; i++) {
		
		if((child_pid = fork())== 0) {
				
	for(int c = 0; c < numofit; c++) {
	

			(*nolockcount)++;
		} if(child_pid == -1) {
			perror("Fork");
		}
		
	
		
	
	
			
exit(0);

	}	
 
}
for (int i = 0; i < 16; i++) {
	wait(NULL);
}
printf("Number of Iterations * Children : %d \n ", numofit * numofchild);
printf("With spinlock mutex: %d \n", (*lockcount));
printf("Without spinlock mutex: %d \n", (*nolockcount));
 return 0;
 
}



