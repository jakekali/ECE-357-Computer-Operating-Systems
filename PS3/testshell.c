
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include <string.h>
#include<sys/wait.h>
#include <errno.h>
#define  bufsiz 10
char *iotokens[3];
int fileredirector(int type, char *iotoken) {
	  char *filename;
    int fd;
    int offset;
    int mode;
    int orig;
 switch(type) {
	case 1: {
		offset = 1;
		mode = O_RDONLY;
		orig = STDIN_FILENO;
		break;
	}
    case 2: {
	offset = 1;
	mode = O_WRONLY | O_CREAT | O_APPEND;
	orig = STDOUT_FILENO;
    break;
    }
    default: 
	return -1;
 }
     if((fd = open(iotoken+offset, mode, 0666)) < 0) {
        fprintf(stderr, "Error 1:", strerror(errno));
        return -1;
    }
    if(dup2(fd, orig) < 0) {
        fprintf(stderr, "Error 2:", strerror(errno));
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
	char *token = strtok(line, " ");
	char **tokens = malloc(100 * sizeof(char *));
	//check error
	int pos = 0;
    int iopos = 0;
	while(token != NULL) {
        if(fakelex(token) > 0) {
            iotokens[iopos] = token;
            iopos++;
            break;
        }
		tokens[pos] = token; 
		token = strtok(NULL, " ");
		pos++;
	}
	tokens[pos] = NULL;
	return tokens;
};
void execute(char **tokens) {
	int status;
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
	wait(NULL);
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


