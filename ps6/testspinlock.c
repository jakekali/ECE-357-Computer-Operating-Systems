#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "spinlock.h"
#include <errno.h>
#include <string.h>
int main(int argc, char **argv) {
	int *lockcount = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	int  *nolockcount = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	int status = 0;
	struct spinlock *l = mmap(NULL, sizeof(struct spinlock), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	if((lockcount || nolockcount || l) == 0) {
		fprintf(stderr, "Error with mmap  : %s. \n",strerror(errno) );
	}
	if (argc == 1) {
		fprintf(stderr, "Usage ./a.out <numberofiterations> * <nchild> \n");
		exit(-1);
	}
	int numofit = atoi(argv[1]);
	int numofchild = atoi(argv[2]);
	
	l->lock = 0;
	pid_t child_pid;
	for (int i = 0; i < numofchild; i++) {
		
		if((child_pid = fork())== 0) {
				
	for(int c = 0; c < numofit; c++) {
		spin_lock(l);
		(*lockcount)++;
			spin_unlock(l);

	
		}
		
	
		
	
	
			
exit(0);

	}	
	if(child_pid == 1) {
		perror("fork");
	}
 
}


	for (int i = 0; i < numofchild; i++) {
		
		if((child_pid = fork())== 0) {
				
	for(int c = 0; c < numofit; c++) {


			(*nolockcount)++;
		}
		
			
exit(0);

	}	
	if(child_pid == 1) {
		perror("fork");
	}
 
}

for (int i = 0; i < numofchild*2; i++) {
	wait(NULL);
}
printf("Number of Iterations * Children : %d \n ", numofit * numofchild);
printf("With spinlock mutex: %d \n", (*lockcount));
printf("Without spinlock mutex: %d \n", (*nolockcount));
 return 0;
 
}



