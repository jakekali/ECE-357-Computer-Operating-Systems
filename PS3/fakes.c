#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include <string.h>
#include<sys/wait.h>
#include <linux/limits.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#define LOW8BIT(x) (x & 0xFF)
#define  bufsiz 10
int status = 0;
char *iotokens[3];
int fileredirector(int type, char *iotoken) {
	char *filename;
    int fd;
    int offset;
    int mode;
    int orig;
	switch(type) 
	{
		case 1: 
		{
			offset = 1;
			mode = O_RDONLY;
			orig = STDIN_FILENO;
			break;
		}
		case 2: 
		{
			offset = 1;
			mode = O_WRONLY | O_CREAT | O_TRUNC;
			orig = STDOUT_FILENO;
			break;
		}
		case 3: 
		{
			offset = 2;
			mode = O_WRONLY | O_CREAT | O_TRUNC;
			orig = STDERR_FILENO;
			break;
		}
		case 4: {
			offset = 3;
			mode =  O_WRONLY | O_CREAT | O_APPEND;
			orig = STDOUT_FILENO;
			break;
		}
		case 5: {
			offset = 4;
			mode =  O_WRONLY | O_CREAT | O_APPEND;
			orig = STDERR_FILENO;
			break;
		}
		default: 
			return -1;
	}
	if((fd = open(iotoken+offset, mode, 0666)) < 0) 
	{
		fprintf(stderr, "Error no such file or directory : %s \n", iotoken);
		return -1;
	}
    if(dup2(fd, orig) < 0) 
	{
        fprintf(stderr, "Error with redirection %s", strerror(errno));
        return -1;
    }
    close(fd);
	return -1;

}

int fakelex(char * token) {
	if(strlen(token) < 3) {
		return 0;
	}
	switch(token[0]) {
		case '<':
			return 1;
			break;
		case '>':
			if(token[1] == '>') {
				return 4;
			} else {
				return 2;
			}
			break;
		case '2': {
				  if(token[1] == '>' && token[2] == '>') {
					  return 5;
				  }
				  else if(token[1] == '>') {
					  return 3;
				  } else {
					  return 0;
				  }
				  break;

			  }
        default :
				  return 0;
	} 
}
char **tokenizer(char * line, int len) {
	
	char * token;
	char **tokens = malloc(100 * sizeof(char *));
	token = strtok(line, " ");
	int pos_io = 0;
	int pos = 0;
	while(token != NULL) {
		int type = fakelex(token);
		if(type != 0) {
			iotokens[pos_io] =  token;
			pos_io++;
		}else{
			tokens[pos] = token;
			pos++;
		}
		token = strtok(NULL, " ");
	}
	return tokens;
}
void execute(char **tokens) {

	struct rusage usage;
	struct timeval current_time;
	struct timeval end_time;
	gettimeofday(&current_time, NULL);       
  
	int pid = fork();
if(fork() == 0) {
        int i = 0;
        while(iotokens[i]) {
            fileredirector(fakelex(iotokens[i]), iotokens[i]);
            i++;
        }
		if(execvp(tokens[0], tokens) == -1) {
			fprintf(stderr, "Error : %s \n", strerror(errno));
		}
	}
if(wait3(&status, 0, &usage) < 0) {
			fprintf(stderr, "Error with waiting for child process statistics : %s", strerror(errno));
		} 
			gettimeofday(&end_time, NULL);
		fprintf(stderr, "Child process %d ", pid);
		if(WIFEXITED(status)) {
			fprintf(stderr, " exited normally with return %d\n", WEXITSTATUS(status));
		} else if(WIFSIGNALED(status)) {
			if(WCOREDUMP(status)) {
				fprintf(stderr ,"exited with signal %d ", WTERMSIG(status));

				fprintf(stderr,"(segmentation fault) \n");
			} else {
				fprintf(stderr, "exited with signal %d \n", WTERMSIG(status));
			}
		}

		fprintf(stderr, "Real time: %.3fs ", (double)((end_time.tv_sec - current_time.tv_sec) + ((double)(end_time.tv_usec - current_time.tv_usec)/CLOCKS_PER_SEC)));
		fprintf(stderr, "User time: %ld.%.03lds ", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec/1000);
		fprintf(stderr, "System time: %ld.%.03lds \n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec/1000);


	for(int i = 0; iotokens[i]; i++){
		iotokens[i] = NULL;
	}
}


int main(int argc, char *argv[]) {
	FILE *fp;
	char *line = NULL;
	char **tokens;
	ssize_t len = 0;
	if (argc == 1) {
		fp = stdin;
	} else {
		fp = fopen(argv[1], "r");
	}
	while(1) {

		if(fp == stdin)  {
			printf("%% ");
		}
		if(getline(&line, &len, fp) != -1) {
			if(line[0] == '#' || line[0] == '\n')
			{
				continue;
			}

			line[strlen(line) -1] = '\0';   
			tokens = tokenizer(line, len);
			execute(tokens);

		} else {

			exit(0);
		}

	}

	return 0;
}
