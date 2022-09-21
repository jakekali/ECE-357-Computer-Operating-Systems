#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


#define FILE_NAME "test"

int main(int argc, char *argv[])
{
   struct stat sb;
   mode_t file_permision; 

   if (stat(FILE_NAME, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

   file_permision = sb.st_mode; 
   printf(" File permission : %o (octal)\n",
            (unsigned int) file_permision); 
  return 0;
}

