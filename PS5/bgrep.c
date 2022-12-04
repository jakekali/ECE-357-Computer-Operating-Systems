#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include<setjmp.h>

jmp_buf int_jb;


int processFile(char * pattern, int pattern_size, char *filename, int pattern_context) {
    // mmap the input file
    char * file;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Error input opening file");
        return -1;
    }

    int fsize;
    fsize = lseek(fd, 0, SEEK_END);
    if ((file=mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0))==MAP_FAILED)
        perror("oops");
           


    char * file_start = file;
    char * file_end = file + fsize;
    int fst = fsize;
    char *loc = NULL;
    while(loc = memmem(file, fsize, pattern, pattern_size)){
        int diff = loc - file_start;

            if(pattern_context == 0){
                printf("%s:%d \n", filename, diff);
            }
         
            if(pattern_context) {
            printf("%s:%d", filename, diff);
            char *beg;
            int end;
            char * window = calloc(1, pattern_context + pattern_size + pattern_context + 1);
            if ((beg = loc - pattern_context )< file_start){
                beg = file_start;
            }
             end = pattern_size + 2 * pattern_context;
            if(pattern_context > loc - beg) {
                end = loc - beg + pattern_size + pattern_context; 
               
            }
             if(beg + end > file_end) {
                    end = file_end - beg;
                }
            
           
          //  if (diff + pattern_context + pattern_size + pattern_context > fst){
         //       end = pattern_context + pattern_size +1;
         //   }
            printf(" ");
            memcpy(window, beg, end);
            for(int i = 0; i < end; i++) {
                 if(isprint(window[i]))  {
                 printf(" %c ", window[i]);
                 } else {
                    printf( "? ", loc[i]);
                 }
                 
            }
            printf(" ");
             for(int i = 0; i < end; i++) {
                  printf(" %X ", file[i] & 0xFF);
            }
            printf("\n");
           
            }
            file = loc + 1; 
            fsize = fst - diff - 1;
        
     
    }

    return 0;
}

void sigHandles(int signum){
    optind++ ;
    siglongjmp(int_jb, 1);

}

int main(int argc, char * argv[])
{
    signal(SIGBUS, sigHandles);

    // process input arguments using getopt
    int c; 
    int patternFileFlag = 0;
    char pattern_file[PATH_MAX];
    int pattern_context = 0;

    while ((c = getopt(argc, argv, "p:c:")) != -1)
    {
        switch (c)
        {
            case 'p':
                patternFileFlag = 1;
                strcpy(pattern_file, optarg);
                break;
            case 'c':
                pattern_context = atoi(optarg);
                break;
        }
    }

    char *p; 
    int fsize = 0;

    if(patternFileFlag){
        // mmap the pattern file
        // open the file for reading
        int fd = open(pattern_file, O_RDONLY);
        if (fd == -1) {
            printf("Error opening file");
            return -1;
            }

       

        fsize = lseek(fd, 0, SEEK_END);
    

        if ((p=mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0))==MAP_FAILED)
            perror("oops");
    }

    if(!patternFileFlag) {
        p = malloc(strlen(argv[optind]) + 1);
        fsize = strlen(argv[optind]);
        memcpy(p, argv[optind], strlen(argv[optind]));
        optind++;
    }
    int filesPresent = 0;
    sigsetjmp(int_jb, 1);
    for (; optind < argc; optind++){    
        filesPresent = 1;
        printf("We are searching for pattern %s, in file: %s with context bytes %d \n", p, argv[optind], pattern_context);
        processFile(p, fsize, argv[optind], pattern_context);
    }
    if(!filesPresent){
        int fd = open(STDIN_FILENO, O_RDONLY);
        if (fd == -1) {
            printf("Error opening file");
            return -1;
            }

        fsize = lseek(fd, 0, SEEK_END);
    

        if ((p=mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0))==MAP_FAILED)
            perror("oops");

        processFile(p, fsize, argv[optind], pattern_context);
    }
}


                
