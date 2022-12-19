

#ifndef FIFO_H
#define FIFO_H
#define MYFIFO_BUFSIZ 1024
#include "cv.h"
#include "spinlock.h"
#include <string.h>
struct fifo {
    unsigned long buf[MYFIFO_BUFSIZ];
    int next_write, next_read;
    int item_count;
    struct cv full, empty;
    struct spinlock mutex;
};

void fifo_init(struct fifo *f);
/* Initialize the shared memory FIFO *f including any required underlying
* initializations (such as calling cv_init). The FIFO will have a static
* fifo length of MYFIFO_BUFSIZ elements. #define this in fifo.h.
* A value of 1K is reasonable.
*/
void fifo_wr(struct fifo *f,unsigned long d);
/* Enqueue the data word d into the FIFO, blocking unless and until the
 FIFO has room to accept it. (i.e. block until !full)
* Wake up a reader which was waiting for the FIFO to be non-empty
*/
unsigned long fifo_rd(struct fifo *f);
/* Dequeue the next data word from the FIFO and return it. Block unless
* and until there are available words. (i.e. block until !empty)
* Wake up a writer which was waiting for the FIFO to be non-full
*/
#endif