
#include "spinlock.h"
#include "tas.h"
#include <sched.h>
void spin_lock(struct spinlock *l) {
  while (tas(&(l->lock)) != 0) {
   // sched_yield();
  }
}

void spin_unlock(struct spinlock *l) {
  l->lock = 0;
}


