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
int first_flag = 0;
//int decode() to do take in statname
int walk(char *pathname) {
 

  //lstat in the beginning 
    struct stat statbuf;
    DIR * dir;
    int length;
    char full_path[PATH_MAX + 1];
     if(lstat(pathname, &statbuf) < 0) {
     return -1;
     }
  if(S_ISDIR(statbuf.st_mode) == 0) {
      return 1;
  }
  if(!first_flag) {
  printf("%s", pathname);
  first_flag = 1;
  }
    dir = opendir (pathname);
    strcpy(full_path, pathname);
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
     if (walk(local_path) != 1) {
      break;
       
  }
   
}
    closedir(dir);
}

int main(int argc, char *argv[])
{
    char * pathname = "./testtrav/";
    walk(pathname);
    return 0;
}

