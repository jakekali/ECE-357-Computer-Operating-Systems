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
#include <pwd.h>
#include <time.h>
#include <grp.h>
#include <sys/sysmacros.h>
#include <getopt.h>
#include <ctype.h>

int first_flag = 0;
int MTIME = -1;
int UID = -1; 
char * UID_N;
char *orig_path;

int decode(char *path, struct stat * statbuf) {
  char perms[11];
  memset(perms,0,strlen(perms));

  ino_t inumber = statbuf->st_ino;
  blkcnt_t block_size = (statbuf->st_blocks)/2;
  nlink_t hlinks = statbuf->st_nlink;
  struct passwd *pwd;
  struct group *grp;
  

switch(statbuf->st_mode & S_IFMT) {
  case S_IFREG: perms[0] = '-'; break;
  case S_IFBLK: perms[0] = 'b'; break;
  case S_IFCHR: perms[0] = 'c'; break;
  case S_IFIFO: perms[0] = 'p'; break; 
  case S_IFLNK: perms[0] = 'l'; break;
  case S_IFSOCK: perms[0] = 's'; break;
  case S_IFDIR: perms[0] = 'd'; break;
}
statbuf->st_mode & S_IRUSR ? (perms[1] =  'r') : (perms[1] =  '-'); 
statbuf->st_mode & S_IWUSR ? (perms[2] =  'w') : (perms[2] =  '-'); 
statbuf->st_mode & S_IXUSR ? (perms[3] =  'x') : (perms[3] =  '-'); 
statbuf->st_mode & S_IRGRP ? (perms[4] =  'r') : (perms[4] =  '-'); 
statbuf->st_mode & S_IWGRP ? (perms[5] =  'w') : (perms[5] =  '-'); 
statbuf->st_mode & S_IXGRP ? (perms[6] =  'x') : (perms[6] =  '-'); 
statbuf->st_mode & S_IROTH ? (perms[7] =  'r') : (perms[7] =  '-'); 
statbuf->st_mode & S_IWOTH ? (perms[8] =  'w') : (perms[8] =  '-'); 
statbuf->st_mode & S_IXOTH ? (perms[9] =  'x') : (perms[9] =  '-'); 

if(statbuf->st_mode & S_ISUID){
  if(statbuf->st_mode & S_IXUSR){
    perms[3] =  's';
  }else{
    perms[3] =  'S';
  }
}

if(statbuf->st_mode & S_ISGID ){
  if(statbuf->st_mode & S_IXGRP){
    perms[6] =  's';
  }else{
    perms[6] =  'S';
  }
}

if(statbuf->st_mode & S_ISVTX ){
  if(statbuf->st_mode & S_ISVTX){
    perms[9] =  't';
  }else{
    perms[9] =  'T';
  }
}

if((pwd = getpwuid(statbuf->st_uid)) == NULL) {
  fprintf(stderr, "UID not found");
}
if((grp = getgrgid(statbuf->st_gid)) == NULL) {
  fprintf(stderr, "GID not found");
}
int minor_num = 0;
int major_num = 0;
off_t size;

if(S_ISBLK(statbuf->st_mode) || S_ISCHR(statbuf->st_mode)) {
  int major_num = major(statbuf->st_dev);
  int minor_num = minor(statbuf->st_dev);
} else {
   size = statbuf->st_size;
}
time_t time = (statbuf->st_mtim).tv_sec;




char * print_time = ctime(&time);

if(S_ISLNK(statbuf->st_mode)) {
  char buff[2*(PATH_MAX + 1)];
  memset(buff,0,strlen(buff));
  int len;
  if(len = readlink(path, buff, sizeof(buff)) != 1) {

  strcat(path, " -> ");
  strcat(path, buff);
 }
}

print_time[strlen(print_time) -1 ] = '\0';

if(MTIME != -1){
  if(time >= MTIME){
      return 0;
  }
}
if(UID != -3) {
  if(UID == -2) {
    if(strcmp(getpwnam(UID_N)->pw_name, pwd->pw_name))
    return 0;
  }
  if(UID >= 0) {
 if(strcmp(getpwuid(UID)->pw_name, pwd->pw_name))
  return 0;
  }
}

printf("%d \t", inumber);
printf("%d", block_size);
printf(" %s\t", perms);
printf("   %d", hlinks);
printf(" %s\t", pwd->pw_name);
printf("%s\t", grp->gr_name);
if(S_ISBLK(statbuf->st_mode) || S_ISCHR(statbuf->st_mode)) {
  printf("%d ,  %d  ", major_num, minor_num );
} else {
  printf("%4d  ", size);
}
printf("%s ", print_time);
printf("%s", path);
printf("\n");

} 

int walk(char *pathname) {
	struct stat statbuf;
	DIR * dir;
	int length;
	char full_path[PATH_MAX + 1];
	if(lstat(pathname, &statbuf) < 0) {
    fprintf(stderr, "Error: %s \n", strerror(errno));
		return -1;
	}
  
  decode(pathname, &statbuf);

	if(S_ISDIR(statbuf.st_mode) == 0) {
		return 1;
	}
  
	if((dir = opendir(pathname)) == NULL){
    fprintf(stderr, "Can't Read %s: %s \n", pathname, strerror(errno));
    return -1;
  }

	strcpy(full_path, pathname); 
	strcat(full_path, "/" ); 

	struct dirent *openeddir;
  errno = 0;
	while((openeddir = readdir(dir)) != NULL){
    char* file = openeddir->d_name;
		if (strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
			continue;
		char local_path [PATH_MAX + 1];
		strcpy(local_path, full_path);
		strcat(local_path, file);
    if(lstat(local_path, &statbuf) < 0) {
      fprintf(stderr, "Error: %s \n", strerror(errno));
		  return -1;
    }

		if (walk(local_path) != 1) {
			if(errno == EMFILE)
        break;
      continue;
		}

	}
  if(errno == 0){
    return 0;
  }else{
    fprintf(stderr, "Error: %s \n", strerror(errno));
    return -1;
  }
	closedir(dir);
}


int main(int argc, char *argv[])
{
  int opt;
  UID = -3;
  while((opt = getopt(argc, argv, "m:u:")) != -1) 
 {
   switch (opt) 
   {
    case 'm':

      MTIME = atoi(optarg);
      break;
    case 'u':
      if(isdigit(optarg[0])){
        UID = atoi(optarg);
        if(getpwuid(UID) == NULL) {
          fprintf(stderr, "UID %d not found", UID);
          return -1;
        } 
        break;
      }else{
        UID = -2;
      }
      UID_N = optarg;
      if(getpwnam(UID_N) == NULL) {
        fprintf(stderr, "Username  %s not found.", UID_N);
        return -1;
      }
      break;
   }
 }
if((argc - optind) > 1) {
  fprintf(stderr, "Error : Too many arguments \n");
  return -1;
}
  while(optind < argc) {
  orig_path =  argv[optind++];
  }
  if(orig_path[strlen(orig_path) - 1] == '/') 
  orig_path[strlen(orig_path) - 1] = '\0';
walk(orig_path);

}

