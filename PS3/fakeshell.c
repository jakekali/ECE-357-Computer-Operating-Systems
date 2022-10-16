#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>

int main(int argc, char **argv[])
{

    // Read in from STDIN
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) 
    {
        if(line[0] == '#' || line[0] == '\n')
        {
            continue;
        }

        // Parse the line
        char *token = strtok(line, " ");
        char *args[10];
        int i = 0;
        while(token != NULL)
        {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        // Execute the command
        if(fork() == 0)
        {
            // Establish IO Redirection
            int j = 0;
            while(args[j] != NULL)
            {
                if(strcmp(args[j], ">") == 0)
                {
                    // Redirect STDOUT to file
                    // Open the file
                    int fd; 
                    if( (fd = open(stdout, O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0)
                    {
                        perror("unable to open file");
                        return -1;
                    }

                    if( dup2(fd, STDOUT_FILENO) < 0)
                    {
                        perror("unable to redirect stdout");
                        return -1;
                    }

                    // Close the file
                    close(fd);

                    // Remove the redirection from the args
                    args[j] = NULL;

                } else if(strcmp(args[j], "<") == 0)
                {
                    // Redirect STDIN to file
                    // Open the file
                    int fd;
                    if( (fd = open(args[j+1], O_RDONLY)) < 0)
                    {
                        perror("unable to open file");
                        return -1;
                    }

                    if( dup2(fd, STDIN_FILENO) < 0)
                    {
                        perror("unable to redirect stdin");
                        return -1;
                    }

                    // Close the file
                    close(fd);

                    // Remove the redirection from the args
                    args[j] = NULL;
                }
            }
                j++;
        
            // Execute the command
            execvp(args[0], args);
            perror("execvp failed");
            return -1;
        
        }else
        {
            // Parent
            wait(NULL);
        }


    }

}

