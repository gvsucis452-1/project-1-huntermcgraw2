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

int create_nodes(int node_id, int num_nodes, int fd[][2]);
int send_message(int node_id, int num_nodes, char *header, int fd[][2]);

int main(int argc, char *argv[])
{    
    int k = atoi(argv[1]);
    int fd[k][2];

    pid_t pid = fork();

    if (pid == 0) {
        create_nodes(1, k, fd);
    } else {

        int message_status;
        char message[128];
        char node[128];
        char header[128];
        pipe(fd[0]);

        while (1) {
        printf("Send a message: ");
        scanf("%s", message);
        printf("Send to node: ");
        scanf("%s", node);

        strcpy(header, node);
        strcat(header, message);         
        message_status = send_message(0, k, header, fd);
        }
    }
    
    return 0;
}

int create_nodes(int node_id, int num_nodes, int fd[][2]) {
    if (node_id >= num_nodes) {
        return 1;
    }
    pipe(fd[node_id]);
    pid_t pid_next = fork();
    if (pid_next == 0) {
        create_nodes(node_id + 1, num_nodes, fd);
    } else {

    }
}

int send_message(int node_id, int num_nodes, char *header, int fd[][2]) {
    char input[128];
    char output[128];

    if (node_id >= num_nodes) {
        exit(0);
    }

    if (node_id = 0) {
        read(fd[num_nodes-1][0], input, sizeof(input));
    } else {
        read(fd[node_id][0], input, sizeof(input));
    }
    printf("Node %d received [%s]\n", node_id, input);
    strcpy(output, input);
    write(fd[num_nodes][1], output, sizeof(output));
    printf("Node %d wrote [%s]\n", node_id, output);

    return 1;
}