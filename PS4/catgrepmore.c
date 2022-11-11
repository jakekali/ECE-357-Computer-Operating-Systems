#define _GNU_SOURCE
#include <stdio.h> 
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <signal.h>
#include <stdlib.h>
#include<setjmp.h>
# include<errno.h>


#define READ 0
#define WRITE 1

char *pattern;
int i = 2;
int fd_file = -1;
jmp_buf int_jb;
pid_t grepChild, moreChild;



// Establishing Pipes
int fd_m2g[2];
int fd_g2mr[2];

int bytes_wrote = 0;


void nextFile(int signum){

    if (fd_file != -1){
        close(fd_file);
    }

    if (signum != SIGPIPE) {
        kill(grepChild, SIGKILL);
        kill(moreChild, SIGKILL);
    }
   
    
    close(fd_m2g[WRITE]);
    i++;

    longjmp(int_jb, 1);

}

void printData(int signum){
    fprintf(stderr, "We have wrote %d bytes to the pipe, and have proccessed %d files.",  bytes_wrote, i - 1);
}


int establishPipe(){

    pipe(fd_m2g);
    pipe(fd_g2mr);
    // TODO Check for Errors


    if((pipe(fd_m2g) == -1) || (pipe(fd_g2mr) == -1)){
        perror("Pipe");
        return -1;
    }


    switch(grepChild = fork()){
        case -1:
            perror("Fork");
            return -1;

        case 0: // Child 1: GREP

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
            break;
    }

    switch(moreChild = fork()){
        case -1:
            perror("Fork");
            return -1;

        case 0: // Child 2: MORE

            // Closing unused pipes
            close(fd_m2g[READ]);
            close(fd_m2g[WRITE]);
            close(fd_g2mr[WRITE]);

            // Redirecting STDIN
            dup2(fd_g2mr[READ], STDIN_FILENO);

            // Executing MORE
            execlp("more", "more", NULL);

            // Closing used pipes
            close(fd_g2mr[READ]);
            // Exit
            exit(0);

        default: 
            break;
    }

    // Closing unused pipes
    close(fd_g2mr[READ]);
    close(fd_g2mr[WRITE]);
    close(fd_m2g[READ]);
}


int main(int argc, char *argv[]) {

      pattern = argv[1];
      

    // Establish Signal Handler for SIGUSR1
    signal(SIGUSR1, printData);
    signal(SIGPIPE, nextFile);
    signal(SIGUSR2, nextFile);
 

    struct sigaction snf;
    snf.sa_handler = nextFile;
    snf.sa_flags = SA_RESTART;

    struct sigaction spF;
    spF.sa_handler = printData;
    spF.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &spF, NULL) == -1 || sigaction(SIGUSR2, &snf, NULL) == -1 || sigaction(SIGPIPE, &snf, NULL) == -1){  
        fprintf(stderr, "Error while setting signal: %s\n", strerror(errno));
        exit(1);
    } 

sigsetjmp(int_jb, 1);


  

    // read from a file and write to fd_m2g[WRITE]
   
    while(i < argc){
      
        establishPipe();

        char *filename = argv[i];
        fd_file = open(filename, O_RDONLY);
        if(fd_file == -1){
            perror("Open");
            return -1;
        }

        while(1){
            char buffer[4096];
            memset(buffer, '\0', 4096);
            int n = read(fd_file, buffer, 4096);
            if(n == 0 || n == -1){
                break;
            }
            bytes_wrote += write(fd_m2g[WRITE], buffer, n);        
        }

        wait(NULL);
        wait(NULL);

        close(fd_file);
        close(fd_m2g[WRITE]);
       
        i++;
    }
   // close(fd_file);
    // Closing used pipes
    wait(NULL);
    wait(NULL);



}