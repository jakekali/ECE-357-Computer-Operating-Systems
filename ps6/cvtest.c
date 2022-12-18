#include "cv.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <errno.h>
int main() {
struct cv *test = mmap(NULL, sizeof(struct cv), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
struct spinlock *test_lock = mmap(NULL, sizeof(struct spinlock), PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);


pid_t childpid[5];
cv_init(test);
printf("wait count (0) %d \n", test->wait_list[0]);
for (int i = 0; i < 5; i++){
    if((childpid[i] = fork()) == 0){
        spin_lock(test_lock);
        cv_wait(test, test_lock);
        fprintf(stderr, "I've left the wait, it my time to die pid# %d \n", getpid());
        // DO SOMETHING
        spin_unlock(test_lock);
        exit(0);
    }
}
   // sleep(2);
while(test->wait_count != 5){
    fprintf(stderr,"wait count %d\n", test->wait_count);
}
    
fprintf(stderr, "the final wait count is %d \n", test->wait_count);


cv_broadcast(test);


    for(int j = 0; j < 5; j++){
    fprintf(stderr, "IN THE WAIT LIST: %d \n", test->wait_list[j]);

    }
    fprintf(stderr, "THE WAIT COUNT IS: %d \n", test->wait_count);
    return 0;

    
int status;
    for (int j = 0; j < 5; j++) {
       wait(NULL);
    }
    printf("wait list %d \n", test->wait_list);
}