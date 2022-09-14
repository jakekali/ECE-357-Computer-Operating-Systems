#include <stdio.h>
#include "mystdiolib.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>





int tabstop(struct MYSTREAM * file_in, struct MYSTREAM * file_out){
    int c;
    while((c = myfgetc(file_in)) != EOF) {
        if(c == '\t') {
    for(int count = 0; count < 4; count++)
            myfputc(' ', file_out);
        } else {
         myfputc(c, file_out);
        }
    

    }
    if((c == -1) && (errno != 0)) 
    fprintf(stderr, "Error: %s\n", strerror(errno));
    if(myfclose(file_in) == -1) fprintf(stderr, "Error:%s\n", strerror(errno) );
    if(myfclose(file_out) == -1) fprintf(stderr, "Error:%s\n", strerror(errno) );
    
return 0;

}

int main(int argc, char *argv[])
{
    struct MYSTREAM * filein;
    struct MYSTREAM * fileout;


    if (argc > 4 || argc < 1){
        fprintf(stderr, "Wrong number of arguments\n");
        return 255;
    }
    
    if(argc == 4){
        if(argv[1][0] == '-' && argv[1][1] == 'o') {
            
            filein = myfopen(argv[3], O_RDONLY, 4096);
            fileout = myfopen(argv[2], O_WRONLY, 4096);
                if(filein == NULL || fileout == NULL) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            return 255;
        }
            return tabstop(filein, fileout);
            
        }else{
            fprintf(stderr, "Invalid argument(s) supplied.\n");
            return 255;
        }
        
    }

    if(argc == 3){
        if(argv[1][0] == '-' && argv[1][1] == 'o') {
        filein = 0;

        filein = myfdopen(0, O_RDONLY, 4096);
        fileout = myfopen(argv[2], O_WRONLY, 4096);
    if(filein == NULL || fileout == NULL) {
            fprintf(stderr, "Error : %s\n", strerror(errno));
            return 255;
        }
        return tabstop(filein, fileout);
        }
           fprintf(stderr, "Invalid argument(s) supplied.\n");
            return 255;
    
    }
    if(argc == 2){
        filein = myfopen(argv[1], O_RDONLY, 4096);
        fileout = myfdopen(1, O_WRONLY, 4096);
    if(filein == NULL || fileout == NULL) {
            fprintf(stderr, "Error : %s\n", strerror(errno));
            return 255;
        }
        
    
        return tabstop(filein, fileout);
        
    }
    if(argc == 1){
        filein = myfdopen(0, O_RDONLY, 4096);
        fileout = myfdopen(1, O_WRONLY, 4096);
     if(filein == NULL || fileout == NULL) {
            fprintf(stderr, "Error : %s\n", strerror(errno));
            return 255;
        }
        return tabstop(filein, fileout);
        
    }
    
}