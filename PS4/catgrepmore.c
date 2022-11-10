
#include <stdio.h> 
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  
#include <unistd.h>
#include <string.h>


int nextFile(int signum){

}

int i = 2;
int fd_file = -1;

int main(int argc, char *argv[]) {

    char *pattern = argv[1];

    // Establishing Pipes
    int fd_m2g[2];
    int fd_g2mr[2];

    pipe(fd_m2g);
    pipe(fd_g2mr);
    // TODO Check for Errors
   

    
    if(fork()){
        if(fork()){
            printf("SOME BS IN THE PARENT");
            write(fd_m2g[1], "bufbufbufbufbufbuf", 4);
            //Parent
           // close(fd_m2g[0]);
            // close(fd_g2mr[1]);
            // close(fd_g2mr[0]);

            // for(; argv[i]; i++){
             //   fd_file = open(argv[i], O_RDONLY);
                // char buf[4096];
                // while(read(fd_file, buf ,4096)){
                    // write(fd_m2g[1], "buf", 4);
                    // memset(buf, '\0', 4096);
                // }
             //   close(fd_file);
            // }
            // close(fd_m2g[1]);
        }else{
            // // MORE CHILD
            // close(fd_m2g[1]);
            // close(fd_m2g[0]);
            // close(fd_g2mr[1]);

            // dup2(fd_g2mr[0], STDIN_FILENO);
                char buf1[4096];
                memset(buf1,'\0',4096);
                read(fd_m2g[0], buf1, 4096);
                printf("%s", buf1);

            // char *arg_list[] =  {"more", NULL};
            // execvp("more", arg_list);
        }
    }else{
        // GREP CHILD
        close(fd_m2g[1]);
        close(fd_g2mr[0]);

        // dup2(fd_m2g[0], STDIN_FILENO);
        // dup2(fd_g2mr[1], STDOUT_FILENO);

        char buf1[4096];
        memset(buf1,'\0',4096);
        read(fd_m2g[0], buf1, 4096);
        write(fd_g2mr[1], buf1, 4096);


        // char *arg_list[] =  {"grep", pattern ,NULL};
        // execvp("grep", arg_list);


    }

}