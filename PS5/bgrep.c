#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>

int processFile(char *filename, char *pattern) {

}

int main(int argc, char * argv[])
{
    // process input arguments using getopt
    int c; 
    int patternFileFlag = 0;
    char pattern_file[PATH_MAX];
    int pattern_context = 5;

    while ((c = getopt(argc, argv, "p:c:")) != -1)
    {
        switch (c)
        {
            case 'p':
                patternFileFlag = 1;
                strcpy(pattern_file,optarg);
                break;
            case 'c':
                pattern_context = atoi(optarg);
                break;
        }
    }

    // mmap the pattern file
    // open the file for reading
    FILE *fp = fopen(pattern_file, "r");
    if (fp == NULL) {
        printf("Error opening file");
        return -1;
        }

    off_t fsize;

    fsize = lseek(fp, 0, SEEK_END);
    printf("File size is %d", fsize);



    // int firstItemFlag = 1;
    // for ( ; optind < argc; optind++) {
    //     if(!patternFileFlag && firstItemFlag) {
    //         pattern = argv[optind];
    //         firstItemFlag = 0;
    //     }
    //     printf("Processing file %s  with pattern %s, context %d, and file %s \n", argv[optind], pattern_context, argv[optind]);
    // }

}


                
