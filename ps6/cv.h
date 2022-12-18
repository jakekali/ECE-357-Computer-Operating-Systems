


#include "spinlock.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>  
struct cv{
    struct spinlock lock;
    pid_t wait_list[64];
    int wait_count;
};
void cv_init(struct cv *cv);
/* Initialize any internal data structures in cv so that it is ready for
* use. The initial condition is that nobody is waiting for this cv.
* You can probably arrange your struct cv so that all-0 bytes is
* the initialization condition.
*/
void cv_wait(struct cv *cv, struct spinlock *mutex);
/* This must be called with the spinlock mutex held by the caller (otherwise
* results will be undefined). Atomically record within the internals
* of *cv that the caller is going to sleep (the wait list), release the
* mutex, and go to sleep (see text below). The wait list is of static size
* CV_MAXPROC waiters. After waking up, re-acquire the mutex
* before returning to the caller
*/
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