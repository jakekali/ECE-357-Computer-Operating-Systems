

#define _GNU_SOURCE
#include "cv.h"
#include <signal.h>
#include <string.h>
void handler(int signum) {
  //  fprintf(stderr, "I have gotten a SIGUSR1 pid# %d \n", getpid());
    return;
}
void cv_init(struct cv *cv) {
    memset(cv, 0, sizeof(cv));
};

void cv_wait(struct cv *cv, struct spinlock *mutex) {
    signal(SIGUSR1, handler);
    sigset_t mask, oldmask, all_butsigusr1;
    sigfillset(&all_butsigusr1);
    sigdelset(&all_butsigusr1, SIGUSR1);
    sigemptyset(&mask);
    sigaddset (&mask, SIGUSR1); //add sig1 to blocked set
    sigprocmask(SIG_BLOCK, &mask, &oldmask); //block SIG1 as it 
    signal(SIGUSR1, handler);
    spin_lock(&cv->lock);
    cv->wait_list[cv->wait_count++] = getpid(); //add process to wait list
    spin_unlock(&cv->lock);
    spin_unlock(mutex);
    sigsuspend(&all_butsigusr1);

    
   // fprintf(stderr, "I woke up and passed the handler # %d \n", getpid());
    if(cv->wait_count > 0) {
        spin_lock(&cv->lock);
        cv->wait_list[--cv->wait_count] = 0;
     //   fprintf(stderr, "I removed myself from wait_list # %d \n", getpid());
        spin_unlock(&cv->lock);
    }
    //unblock all SIGUSR1
     sigprocmask (SIG_UNBLOCK, &mask, NULL);
     spin_lock(mutex);
    // fprintf(stderr, "locked mutex pid # %d \n", getpid());
    
/* This must be called with the spinlock mutex held by the caller (otherwise
* results will be undefined). Atomically record within the internals
* of *cv that the caller is going to sleep (the wait list), release the
* mutex, and go to sleep (see text below). The wait list is of static size
* CV_MAXPROC waiters. After waking up, re-acquire the mutex
* before returning to the caller

*/

}
int cv_broadcast(struct cv *cv) {
    spin_lock(&cv->lock);
    if (cv->wait_count == 0) {
            return 0;
    }
    
    for(int i = 0; i < cv->wait_count; i++) {
        
        if(cv->wait_list[i] > 0) {
          
            kill(cv->wait_list[i], SIGUSR1); 
          //    fprintf(stderr, "PID KILLED: %d WAIT_COUNT: %d \n",   cv->wait_list[i], cv->wait_count);
            cv->wait_list[i] = 0;

        }

    }
    spin_unlock(&cv->lock);
    return 0;
}

int cv_signal(struct cv *cv) {
     spin_lock(&cv->lock);
    if(cv->wait_count == 0) {
        return  0;
    }
    kill(cv->wait_list[cv->wait_count - 1], SIGUSR1);
     spin_unlock(&cv->lock);
    return 0;
}
