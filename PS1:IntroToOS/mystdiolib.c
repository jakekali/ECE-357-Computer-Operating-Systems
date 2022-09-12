//implment the functions in mystudiolib.h
#include "mystdiolib.h"
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct MYSTREAM *myfopen(const char *pathname, int mode, int buffsiz){
    int fd;
    errno = 0;
    if(buffsiz < 1 ||  (mode != O_WRONLY && mode != O_RDONLY)){
        errno = EINVAL;
        return NULL;
    }

    struct MYSTREAM * stream = (struct MYSTREAM  *) malloc(sizeof(struct MYSTREAM ));
    if(errno == ENOMEM){
            //the file does not exist
            return NULL;
        }


    
    //check if file exists than create otherwise open
    
    if(mode == O_WRONLY){

        fd = open(pathname, mode | O_TRUNC);
        if(errno == ENOENT){
            //the file does not exist
            fd = open(pathname, O_WRONLY | O_CREAT , 0777);
        }
        
    }else if(mode == O_RDONLY){

        fd = open(pathname, mode);
        if(errno == ENOENT){
            //the file does not exist
            return NULL;
        }
        
    }else{
        errno = EINVAL;
        return NULL;
    }

    
	stream->fd = fd;
	stream->buffsiz = buffsiz;
	stream->pos = 0;
    stream->mode = mode;
	return stream;

    
}

struct MYSTREAM *myfdopen(int fd, int mode, int buffsiz){
    errno = 0;
    if(buffsiz < 1 || (mode != O_WRONLY && mode != O_RDONLY)){
        errno = EINVAL;
      
        return NULL;
    }
    
    struct MYSTREAM * stream = (struct MYSTREAM  *) malloc(sizeof(struct MYSTREAM ));
    if(errno == ENOMEM){
            //the file does not exist
            return NULL;
        }
    
	stream->fd = fd;
	stream->buffsiz = buffsiz;
	stream->pos = 0;
    stream->mode = mode;
	return stream;
}

int myfgetc(struct MYSTREAM *stream){

    int readrt;
    if(stream->buf[stream->pos] == '\0'){
        stream->pos = 0;
        readrt = read(stream->fd, stream->buf, stream->buffsiz);
    }
    if(readrt == 0 && errno == 0)
        return -1;
    if(errno != 0) {
        return -1;
    }
        
    stream->pos = stream->pos + 1;



    return stream->buf[stream->pos - 1];


}

int myfputc(int c, struct MYSTREAM *stream){
    stream->buf[stream->pos] = c;
    if(stream->pos == stream->buffsiz - 1){
        int wr;
        stream->pos = -1;
        wr = write(stream->fd, stream->buf, stream->buffsiz);
        if(wr == -1 || wr == 0){
            return -1;
        }
    }
     stream->pos = stream->pos + 1;

    return c;
}

int myfclose(struct MYSTREAM *stream){
    int closert;
    if(stream->mode == O_RDONLY) {
       closert = close(stream->fd);
        free(stream);
    }
    if(stream->mode == O_WRONLY) {
        int wr;
        wr = write(stream->fd, stream->buf, stream->pos);
        if(wr == -1 || wr == 0){
            return -1;
        }
       closert =  close(stream->fd);
        free(stream);

           

    }
    if(closert == 0)
    return 0;
    
    return 1;    
} 
/*
int main() {
    struct MYSTREAM * fp;
    int c;
    fp = myfopen("test.txt", O_RDONLY, 4096);
    int count = 0;
    while((c = myfgetc(fp)) != EOF) {
        printf("%d",count);
        count++;
        printf("%c \n", c);

    }
}
*/
