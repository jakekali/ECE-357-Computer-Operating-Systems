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
int *argc_count;


char *filename;
// Establishing Pipes
int fd_m2g[2];
int fd_g2mr[2];
int grepcount = 1;
int bytes_wrote = 0;


void nextFile(int signum){
if(grepcount < i - 1) {
    //checking if file was all ready closed and write side of the pipe was closed
    if (fd_file != -1){
        if( close(fd_file) != 0) {
            fprintf(stderr, "Error closing %s:  %s \n", filename, strerror(errno));
        }

    }
     if( close(fd_m2g[WRITE]) != 0) {
            fprintf(stderr, "Error  (close): %s \n", strerror(errno));
        }
}
     
    if (signum != SIGPIPE) {
        if(grepcount < i - 1) {
            //checking if grepChild still exists
        if(  kill(grepChild, SIGKILL) != 0 ) {
            fprintf(stderr, "Error (kill): %s \n", strerror(errno));
        }
        }
         if(  kill(moreChild, SIGKILL) != 0 ) {
            fprintf(stderr, "Error (kill): %s \n", strerror(errno));
        }
    } 
    if(grepcount < i - 1) {
    if(waitpid(grepChild, NULL, 0) < 0) {
            fprintf(stderr, "Error with wait  (grep): %s\n", strerror(errno));
    }
    }
    grepcount++;
   if(waitpid(moreChild, NULL, 0) < 0) {
    fprintf(stderr, "Error with wait (more): %s\n", strerror(errno));
   }
    


    i++;
    if(signum == SIGUSR2) {
        if(i < *argc_count) {
            //argc is not accessed here (even though it is global) so we used a reference to argc to check if there 
            //are no more files left
        fprintf(stderr, " \n *** SIGUSR2 received, moving on to file %d ***\n", i - 1 );
        } else {
             fprintf(stderr, " \n *** SIGUSR2 received, No more files left!  ***\n ");
        }
    }
  
    longjmp(int_jb, 1);

}

void printData(int signum){
    fprintf(stderr, "\nWe have wrote %d bytes to the pipe, and have proccessed %d files.\n",  bytes_wrote, i - 1);
}


int establishPipe(){


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
            if(close(fd_m2g[WRITE]) != 0) {
                fprintf(stderr, "Error (pipe): %s\n", strerror(errno));
            }
            if(close(fd_g2mr[READ]) != 0) {
              fprintf(stderr, "Error (pipe): %s\n", strerror(errno));
            }

            // Redirecting STDIN and STDOUT
            if(dup2(fd_m2g[READ], STDIN_FILENO)  < 0) {
                fprintf(stderr, "Error with redirection : %s\n", strerror(errno));
            }
                    if(dup2(fd_g2mr[WRITE], STDOUT_FILENO)  < 0) {
                fprintf(stderr, "Error with redirection : %s\n", strerror(errno));
            }

            // Executing GREP
          if(execlp("grep", "grep", pattern, NULL) == -1) {
            fprintf(stderr, "Error with exec : %s\n",  strerror(errno));
          }

            // Closing used pipes
            if(close(fd_m2g[READ]) < 0) {
                fprintf(stderr, "Error with (pipe) %s", strerror(errno));
            }
            if(close(fd_g2mr[WRITE]) < 0) {
            fprintf(stderr, "Error with (pipe) %s", strerror(errno));
            }
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
              if(close(fd_m2g[READ])!= 0) {
                fprintf(stderr, "Error (pipe): %s\n", strerror(errno));
            }
            if( close(fd_m2g[WRITE]) != 0) {
              fprintf(stderr, "Error (pipe): %s\n", strerror(errno));
            }
              if( close(fd_g2mr[WRITE]) != 0) {
              fprintf(stderr, "Error (pipe): %s\n", strerror(errno));
            }

            // Redirecting STDIN
            if (dup2(fd_g2mr[READ], STDIN_FILENO) < 0) {
                    fprintf(stderr, "Error  (pipe redirection): %s", strerror(errno));
                            }

            // Executing MORE
            if(execlp("more", "more", NULL) == -1) {
                 fprintf(stderr, "Error with exec : %s\n",  strerror(errno));
            };

            // Closing used pipes
           if( close(fd_g2mr[READ]) != 0) {
            fprintf(stderr, "Error :%s", strerror(errno));
           }
            // Exit
            exit(0);

        default: 
            break;
    }

    // Closing unused pipes
    if (close(fd_g2mr[READ]) != 0) {
        fprintf(stderr, "Error :%s", strerror(errno));
    }
    if(close(fd_g2mr[WRITE]) != 0) {
    fprintf(stderr, "Error :%s", strerror(errno));
    }

    if(close(fd_m2g[READ]) != 0) {
fprintf(stderr, "Error :%s", strerror(errno));
    }
}


int main(int argc, char *argv[]) {
        argc_count = &argc;
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
   if(argc < 3) {
  
    fprintf(stderr, "Error : not enough input arguments.\n");
    exit(EXIT_FAILURE);
   }
    while(i < argc){
     
        establishPipe();

        filename = argv[i];
        fd_file = open(filename, O_RDONLY);
        if(fd_file == -1){

            //kill currently running processes as the program should be exited 
            //since it cant be run anymore
            kill(grepChild, 9);
            kill(moreChild, 9);
            fprintf(stderr, "Error : (%s) %s \n", filename, strerror(errno));
            exit(EXIT_FAILURE);
            
        }

        for(;;) {
            char buffer[4096];
            memset(buffer, '\0', 4096);
            int n = read(fd_file, buffer, 4096);
            if(n == 0 || n == -1){
                break;
            }

            int ret_write = write(fd_m2g[WRITE], buffer, n); 
            if(ret_write != n) {
                fprintf(stderr, "partial write");
            }
            bytes_wrote += ret_write;       
        }

     


        if(close(fd_file) != 0) {
    fprintf(stderr, "Error : %s", strerror(errno));
   }
    
         if(close(fd_m2g[WRITE]) != 0) {
    fprintf(stderr, "Error : %s", strerror(errno));
   }     
        waitpid(grepChild, NULL, 0);
        //if this point is reached grepChild has finished so the files are closed 
        //and the grepProcess should not exist. This is needed 
        grepcount++;
        waitpid(moreChild, NULL, 0);

       
        i++;
    }
  

   

    



}