

#define _GNU_SOURCE
#include "cv.h"
#include <signal.h>
void handler(int signum) {
    ;
}
void cv_init(struct cv *cv) {
    memset(cv, 0, sizeof(cv));
};

void cv_wait(struct cv *cv, struct spinlock *mutex) {
    signal(SIGUSR1, handler);
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset (&mask, SIGUSR1); //add sig1 to blocked set
    sigprocmask(SIG_BLOCK, &mask, &oldmask); //block SIG1 as it 
    signal(SIGUSR1, handler);
    spin_lock(&cv->lock);
    cv->wait_list[cv->wait_count++] = getpid(); //add process to wait list
    spin_unlock(&cv->lock);
    spin_unlock(mutex);
    sigsuspend(&oldmask);
    if(cv->wait_count > 0) {
        spin_lock(&cv->lock);
        cv->wait_list[--cv->wait_count] = 0;
        spin_unlock(&cv->lock);
    }
    //unblock all SIGUSR1
     sigprocmask (SIG_UNBLOCK, &mask, NULL);
     spin_lock(mutex);
    
/* This must be called with the spinlock mutex held by the caller (otherwise
* results will be undefined). Atomically record within the internals
* of *cv that the caller is going to sleep (the wait list), release the
* mutex, and go to sleep (see text below). The wait list is of static size
* CV_MAXPROC waiters. After waking up, re-acquire the mutex
* before returning to the caller

*/

}
int cv_broadcast(struct cv *cv);
/* Wake up any and all waiters (sleepers) on this cv. If there are no waiters
ECE357:Computer Operating Systems PS 6/pg 4 ©2022 Jeff Hakner
* the call has no effect and is not "remembered" for the next time that
* someone calls cv_wait. cv_broadcast must be called with the same mutex
* held that protects cv_wait, as discussed in lecture notes under "Lost
* Wakup", but note that cv_broadcast does not take a mutex as a parameter.
* Return value: the number of sleepers that were awoken.
*/
int cv_signal(struct cv *cv);
/* Exactly the same as cv_broadcast except at most one sleeper is awoken.
* Your choice how to pick which one if more than one candidate
*/