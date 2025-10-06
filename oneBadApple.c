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

void next_node(int node_id, int fd, int num_nodes, int fd_final);

int main(int argc, char *argv[])
{
    int fd[2], fd_final[2];
    int pipe_creation_result;
    pid_t pid, child;
    int status;
    int input;
    int output = 3;
    
    int k = atoi(argv[1]);
    pipe_creation_result = pipe(fd);
    pipe(fd_final);

    pid = fork();

    if (pid == 0) {
        next_node(1, fd[0], k, fd_final[1]);
    } else { 
        write(fd[1], &output, sizeof(int));
        printf("Node 0 wrote [%d]\n",  output);
        read(fd_final[0], &input, sizeof(int));
        printf("Node 0 received [%d]\n", input);
    }
    
    return 0;
}

void next_node(int node_id, int fd, int num_nodes, int fd_final) {
    if (node_id >= num_nodes) {
        exit(0);
    }
    int input;
    int output = rand() % 5;
    read(fd, &input, sizeof(int));
    printf("Node %d received [%d]\n", node_id, input);
    int fd_next[2];
    int pipe_creation_result_next = pipe(fd_next);
    pid_t pid_next = fork();
    if (pid_next == 0) {
        next_node(node_id + 1, fd_next[0], num_nodes, fd_final);
    } else {
        if (node_id == num_nodes - 1) { 
           write(fd_final, &output, sizeof(int));
        } else {
           write(fd_next[1], &output, sizeof(int));
        }
        printf("Node %d wrote [%d]\n", node_id, output);
    }
}
