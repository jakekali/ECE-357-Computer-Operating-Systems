#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "spinlock.h"
int main() {
	int *lockcount = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	int  *nolockcount = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	int status = 0;
	struct spinlock *l = mmap(NULL, sizeof(struct spinlock), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);

	l->lock = 0;
	pid_t child_pid;
	for (int i = 0; i < 16; i++) {
		
		if((child_pid = fork())== 0) {
				
	for(int c = 0; c < 100000; c++) {
		spin_lock(l);
		(*lockcount)++;
			spin_unlock(l);

			(*nolockcount)++;
		}
		
	
		
	
	
			
exit(0);

	}	
 
}
for (int i = 0; i < 16; i++) {
	wait(NULL);
}
	for (int i = 0; i < 16; i++) {
		
		if((child_pid = fork())== 0) {
				
	for(int c = 0; c < 100000; c++) {
	

			(*nolockcount)++;
		}
		
	
		
	
	
			
exit(0);

	}	
 
}
for (int i = 0; i < 16; i++) {
	wait(NULL);
}
printf("%d \n", (*lockcount));
printf("%d \n", (*nolockcount));
 return 0;
 
}



