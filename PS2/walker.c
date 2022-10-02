/* */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
//int decode() to do take in statname
char walk(char *pathname) {
    struct stat statbuf;
    DIR * dir;
    int length;
    char full_path[PATH_MAX + 1];
    strcpy(full_path, pathname);
    dir = opendir (pathname);
  strcat( full_path, "/" ); //error check
    struct dirent *openeddir;
 while((openeddir = readdir(dir)) != NULL){
     char * file = openeddir->d_name;
    if (strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
            continue;
    printf("\n file  :%s " , file);
       char local_path [PATH_MAX + 1];
       strcpy(local_path, full_path);
      strcat(local_path, file);
        lstat(local_path, &statbuf);
       printf("%s", local_path);
     if (S_ISDIR(statbuf.st_mode)) {
        printf("is a directory \n" );
          
         walk(local_path);
  }
}
    closedir(dir);
}

int main(int argc, char *argv[])
{
    char * pathname = "/home/";
    walk(pathname);
    return 0;
}

