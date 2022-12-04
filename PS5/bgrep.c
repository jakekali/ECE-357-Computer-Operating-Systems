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
#include<errno.h>


jmp_buf int_jb;
int fd; //fd of open file
char *file_glob; 
int sig_ret = 0;
int syscall_fail = 0; //changes to -1 if any of the system call fail
int processFile(char * pattern, int pattern_size, char *filename, int pattern_context) {
	// mmap the input file
	char * file;

	if(filename) {
		fd = open(filename, O_RDONLY);
		if (fd == -1) {
			fprintf(stderr, "Error opening input file %s : %s \n", filename, strerror(errno));
			return -1;
		}
	} else {
		fd = 0;
		filename = "<standard input>";
	}

	int fsize;
	fsize = lseek(fd, 0, SEEK_END);
	if ((file=mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0))==MAP_FAILED){   
        fprintf(stderr, "Error with mmap : %s \n", strerror(errno));
		return -1;
	}
	file_glob = filename;



	char * file_start = file;
	char * file_end = file + fsize;
	int fst = fsize;
	char *loc = NULL;
	int flag_pattern_found = 1;
	while(loc = memmem(file, fsize, pattern, pattern_size)){
		int diff = loc - file_start;
		flag_pattern_found = 0;
        
        printf("%s:%d", filename, diff);

        if(pattern_context == 0) {
            printf("\n");
        }

		if(pattern_context) {
			char *beg;
			int end;
			char * window = calloc(1, pattern_context + pattern_size + pattern_context + 1);
            //check if beggining of file is beyond pattern context
			if ((beg = loc - pattern_context )< file_start){
				beg = file_start;
			}
			end = pattern_size + 2 * pattern_context;
            //check if end of file is beyond pattern context
			if(pattern_context > loc - beg) {
				end = loc - beg + pattern_size + pattern_context; 

			}
			if(beg + end > file_end) {
				end = file_end - beg;
			}
			printf(" ");
			memcpy(window, beg, end);
			for(int i = 0; i < end; i++) {
				if(isprint(window[i]))  {
					printf(" %c ", window[i]);
				} else {
					printf(" ? ");
				}

			}
			printf(" ");
			for(int i = 0; i < end; i++) {
				{
					printf(" %02X ", window[i] & 0xFF);
				}
			}
			printf("\n");

		}
		file = loc + 1; 
		fsize = fst - diff - 1;


	}
	close(fd);


	return flag_pattern_found;
}

void sigHandles(int signum){
    sig_ret = 1;
	optind++;
	fprintf(stderr, "SIGBUS received while processing %s \n", file_glob);
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
	if(argc == 1) {
		fprintf(stderr, "Not enough input arguments. \n");
		return -1;

	}
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
		int fd_pat = open(pattern_file, O_RDONLY);
		if (fd_pat == -1) {
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
    int nofiles = argc - optind;
    int filematchcount = 0;
	sigsetjmp(int_jb, 1);
	for (; optind < argc; optind++){ 
		filesPresent = 1;
		int ret_no;
		if(ret_no = processFile(p, fsize, argv[optind], pattern_context)) {
               if(ret_no == -1) {
                syscall_fail = ret_no;
               } else {
                filematchcount++;
               }

		}
	}
	if(!filesPresent){
		int ret_no;
        nofiles = 1;
		if(ret_no = processFile(p, fsize, NULL, pattern_context)) {
             if(ret_no == -1) {
                syscall_fail = ret_no;
               } else {
                filematchcount++;
               }

		}

	}

	free(p);
    return syscall_fail ? syscall_fail : (filematchcount == nofiles);
  
}



