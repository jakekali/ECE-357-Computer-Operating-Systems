//implment the functions in mystudiolib.h
#include "mystdiolib.h"
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct MYSTREAM *myfopen(const char *pathname, int mode, int bufsiz){
    int fd;
    errno = 0;
    if(bufsiz < 1 ||  (mode != O_WRONLY && mode != O_RDONLY)){
        errno = EINVAL;
        return NULL;
    }

    struct MYSTREAM * stream = (struct MYSTREAM  *) malloc(sizeof(struct MYSTREAM ));
        //malloc failure
    if(stream == NULL){
            errno = ENOMEM; 
            return NULL;
        }

    stream->buf = malloc(bufsiz);
    //malloc failure
    if(stream->buf == NULL){
            errno = ENOMEM; 
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
	stream->bufsiz = bufsiz;
	stream->pos = 0;
    stream->mode = mode;
	return stream;

    
}

struct MYSTREAM *myfdopen(int fd, int mode, int bufsiz){
    errno = 0;
    if(bufsiz < 1 || (mode != O_WRONLY && mode != O_RDONLY)){
        errno = EINVAL;
      
        return NULL;
    }
    
    struct MYSTREAM * stream = (struct MYSTREAM  *) malloc(sizeof(struct MYSTREAM ));
    if(stream == NULL){
            errno = ENOMEM; 
            return NULL;
        }
    stream->buf = malloc(bufsiz);
    //malloc failure
    if(stream->buf == NULL){
            errno = ENOMEM; 
            return NULL;
        }
	stream->fd = fd;
	stream->bufsiz = bufsiz;
	stream->pos = 0;
    stream->mode = mode;
	return stream;
}

int myfgetc(struct MYSTREAM *stream){

    int readrt;
    if(stream->buf[stream->pos] == '\0'){
        stream->pos = 0;
        readrt = read(stream->fd, stream->buf, stream->bufsiz);
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
    if(stream->pos == stream->bufsiz - 1){
        int wr;
        stream->pos = -1;
        wr = write(stream->fd, stream->buf, stream->bufsiz);
        if(wr == -1 || wr == 0 || (wr < stream->bufsiz)){
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
       free(stream->buf);
        free(stream);
     
    }
    if(stream->mode == O_WRONLY) {
        int wr;
        wr = write(stream->fd, stream->buf, stream->pos);
        if(wr == -1 || wr == 0){
            return -1;
        }
       closert =  close(stream->fd);
       free(stream->buf);
        free(stream);

           

    }
    if(closert != 0) {
    return -1;
    }
    
    return 0;    
} 

/*
 int main() {
    struct MYSTREAM * file_in;
        struct MYSTREAM * file_out;
       file_in = myfdopen(0, O_RDONLY, 4096);
        file_out = myfopen("test.txt", O_WRONLY, 4096);
        int c;
        while((c = myfgetc(file_in)) != EOF) {
        if(c == '\t') {
    for(int count = 0; count < 4; count++)
            myfputc(' ', file_out);
        } else {
         myfputc(c, file_out);
        }
    

    }

}
*/





