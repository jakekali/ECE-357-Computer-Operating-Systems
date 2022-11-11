
#include <stdio.h> 
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdlib.h>

int nextFile(int signum){

}

int i = 2;
int fd_file = -1;

#define READ 0
#define WRITE 1


int main(int argc, char *argv[]) {

    char *pattern = argv[1];

    // Establishing Pipes
    int fd_m2g[2];
    int fd_g2mr[2];

    pipe(fd_m2g);
    pipe(fd_g2mr);
    // TODO Check for Errors

    pid_t child1, child2;

    if((pipe(fd_m2g) == -1) || (pipe(fd_g2mr) == -1)){
        perror("Pipe");
        return -1;
    }

    switch(child1 = fork()){
        case -1:
            perror("Fork");
            return -1;

        case 0: // Child 1: GREP
            printf("Child 1: GREP\n");

            // Closing unused pipes
            close(fd_m2g[WRITE]);
            close(fd_g2mr[READ]);

            // Redirecting STDIN and STDOUT
            dup2(fd_m2g[READ], STDIN_FILENO);
            dup2(fd_g2mr[WRITE], STDOUT_FILENO);

            // Executing GREP
            execlp("grep", "grep", pattern, NULL);

            // Closing used pipes
            close(fd_m2g[READ]);
            close(fd_g2mr[WRITE]);
            // Exit
            exit(0);

        default: 
            // Parent - DO NOTHING
            printf("Parent: DO NOTHING\n");
            break;
    }

    switch(child2 = fork()){
        case -1:
            perror("Fork");
            return -1;

        case 0: // Child 2: MORE
            printf("Child 2: MORE\n");

            // Closing unused pipes
            close(fd_m2g[READ]);
            close(fd_m2g[WRITE]);
            close(fd_g2mr[WRITE]);

            // Redirecting STDIN
            dup2(fd_g2mr[READ], STDIN_FILENO);

            // Executing MORE
            execlp("more", "more", NULL);

            // // read from fd_g2mr[READ] and write to STDOUT
            // while(1){
            //     char buffer[1024];
            //     int n = read(fd_g2mr[READ], buffer, 1024);
            //     if(n == 0){
            //         break;
            //     }
            //     buffer[n] = '\0';
            //     printf("MORE: %s", buffer);
            // }

            // Closing used pipes
            close(fd_g2mr[READ]);
            // Exit
            exit(0);

        default: 
            // Parent - DO NOTHING
            printf("Parent: DO NOTHING\n");
    }

    // Closing unused pipes
    close(fd_g2mr[READ]);
    close(fd_g2mr[WRITE]);
    close(fd_m2g[READ]);


    // read from a file and write to fd_m2g[WRITE]
    while(i < argc){
        char *filename = argv[i];
        fd_file = open(filename, O_RDONLY);
        if(fd_file == -1){
            perror("Open");
            return -1;
        }

        while(1){
            char buffer[4096];
            int n = read(fd_file, buffer, 4096);
            if(n == 0){
                break;
            }
            write(fd_m2g[WRITE], buffer, n);
            buffer[n] = '\0';
        }
        close(fd_file);
        i++;
    }
    

    // Closing used pipes
    close(fd_m2g[WRITE]);
 

    wait(NULL);
    wait(NULL);

}