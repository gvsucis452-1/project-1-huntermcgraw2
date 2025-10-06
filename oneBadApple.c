/**********************************************************
 *
 * oneBadApple.c
 * CIS 451 Project 1 (F25)
 *
 * Hunter McGraw
 *************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int fd[2];
    int pipe_creation_result;
    pid_t pid, child;
    int status;
    int output = 3;
    int input;
    
    int k = atoi(argv[1]);
    
    for (int i = 0; i < k; i++) {
        pipe_creation_result = pipe(fd);
        pid = fork();
        
        if (pid == 0) {
            // Child process
            read(fd[0], &input, sizeof(int));
            printf("Node %d received [%d]\n", k + 1, input);
        } else {
            write(fd[1], &output, sizeof(int));
            printf("Node %d wrote [%d] to child process\n", k,  output);
        }

    }
    
    return 0;
}