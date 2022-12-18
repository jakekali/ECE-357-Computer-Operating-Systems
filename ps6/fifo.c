
#include "fifo.h"

void fifo_init(struct fifo *f) {
    memset(f, 0, sizeof(struct fifo));
    cv_init(&f->empty);
    cv_init(&f->full);
    f->next_read=0;
    f->next_write=0;
    f->item_count=0;
}
/* Initialize the shared memory FIFO *f including any required underlying
* initializations (such as calling cv_init). The FIFO will have a static
* fifo length of MYFIFO_BUFSIZ elements. #define this in fifo.h.
* A value of 1K is reasonable.
*/
void fifo_wr(struct fifo *f,unsigned long d) {
/* Enqueue the data word d into the FIFO, blocking unless and until the
ECE357:Computer Operating Systems PS 6/pg 5 ©2022 Jeff Hakner
* FIFO has room to accept it. (i.e. block until !full)
* Wake up a reader which was waiting for the FIFO to be non-empty
*/
spin_lock(&f->mutex);

// fprintf(stderr, "ITEM COUNT: %d > = %d\n", f->item_count, MYFIFO_BUFSIZ);
// 
while(f->item_count >= MYFIFO_BUFSIZ){
   cv_wait(&f->full, &f->mutex);
}
f->buf[f->next_write++] = d; 
f->next_write%=MYFIFO_BUFSIZ;
f->item_count++;
cv_signal(&f->empty);
spin_unlock(&f->mutex);
}

unsigned long fifo_rd(struct fifo *f) {
    unsigned long d;
   spin_lock(&f->mutex);
        while(f->item_count<=0) {
            cv_wait(&f->empty, &f->mutex);
        }
    d=f->buf[f->next_read++];
    f->next_read%= MYFIFO_BUFSIZ;
    f->item_count--;
     cv_signal(&f->full);
    spin_unlock(&f->mutex);
       
    return d;

}   

//INSPIRED BY THE LECTURE NOTES :)