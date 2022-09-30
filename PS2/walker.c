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

char max_path[1024];
char walk(char *pathname) {
    struct stat statbuf;
    DIR * dir;
    strcpy(max_path, pathname);
    strcat( max_path, "/" );
    dir = opendir (pathname);
    struct dirent *openeddir;
    int i = 0;
    while(openeddir = readdir(dir)){
        char * file = openeddir->d_name;
        if (strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
            continue;
        
        printf("\n file  :%s " , file);
        char local_path [1024];
        strcpy(local_path, max_path);
        strcat(local_path,file);
        lstat(local_path, &statbuf);

    //    printf("%d", strerror(errno));
        
      //  printf("%x", statbuf.st_ino);
       if (S_ISDIR(statbuf.st_mode)) {
            printf("is a directory \n" );
            walk(local_path);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    char string[3]  = "abc";
    printf("%s", string);
    char * pathname = "./testtrav/";
    walk(pathname);
    return 0;
}

