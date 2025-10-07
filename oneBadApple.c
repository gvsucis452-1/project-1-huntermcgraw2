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
#include <errno.h>
#include <signal.h>

void sig_handler(int);

int create_nodes(int node_id, int num_nodes, int fd[][2]);

int send_message(int node_id, int num_nodes, int fd[][2]);

int main(int argc, char *argv[]) {

    signal(SIGINT, sig_handler);

    printf("Created node 0\n");
    int k = atoi(argv[1]);
    int fd[k][2];

    pipe(fd[0]);
    pipe(fd[k - 1]);
    pid_t pid = fork();

    if (pid == 0) {
        close(fd[0][1]);
        printf("Created node 1\n");
    } else {
        char message[128];
        char node[128];
        char header[128];
        char input[128];
        int status;
        
        create_nodes(2, k, fd);

        while (1) {
            printf("Send a message: ");
            fgets(message, sizeof(message), stdin);
            message[strlen(message) - 1] = '\0';
            printf("Send to node: ");
            fgets(node, sizeof(node), stdin);
            node[1] = '\0';

            strcpy(header, node);
            strcat(header, message);
            printf("header: %s\n", header);

            close(fd[0][0]);
            write(fd[0][1], header, sizeof(header));
            printf("Node %d (pid %d)  wrote [%s]\n", 0, getpid(), header + 1);

            status = read(fd[k - 1][0], input, sizeof(input));
            if (status == -1) {
                perror("Failed read");
            }
            printf("Node %d received [%s]\n", 0, input);
        }
        close(fd[0][1]);
        close(fd[k - 1][0]);
    }

    return 0;
}

int create_nodes(int node_id, int num_nodes, int fd[][2]) {
    if (node_id > num_nodes) {
        return 1;
    }
    if (node_id < num_nodes) {
        pipe(fd[node_id - 1]);
    }
    pid_t pid_next = fork();
    if (pid_next == 0) {
        close(fd[node_id - 1][1]);
        printf("Created node %d\n", node_id);
        create_nodes(node_id + 1, num_nodes, fd);
    } else {
        close(fd[node_id - 1][0]);
        send_message(node_id - 1, num_nodes, fd);
    }
}

int send_message(int node_id, int num_nodes, int fd[][2]) {
    char input[128];
    char output[128];
    int status;
    int intended_receiver;

    while (1) {

        //sleep(rand() % 3);

        if (node_id >= num_nodes) {
            exit(0);
        }
        status = read(fd[node_id - 1][0], input, sizeof(input));
        if (status == -1) {
            perror("Failed read");
        }
        intended_receiver = input[0] % 48;
       
        printf("Node %d received [%s]\n", node_id, input);
        
        if (node_id == num_nodes - 1 && intended_receiver > node_id) {
            printf("Destination of %d does not exist in this ring\n", intended_receiver);
            strcpy(output, "");
        } else if (node_id == intended_receiver) {
            printf("I (node %d) am the intended recipient!\n", node_id);
            strcpy(output, "");
        } else {
            strcpy(output, input);
        }
        //sleep(rand() % 3);
        write(fd[node_id][1], output, sizeof(output));
        if (!strcmp(output, "")) { 
           printf("Node %d (pid %d)  wrote [%s]\n", node_id, getpid(), output);
        } else {
           printf("Node %d (pid %d)  wrote [%s]\n", node_id, getpid(), output + 1);
        } 
    }

    close(fd[node_id - 1][0]);
    close(fd[node_id][1]);

    return 1;
}

void sig_handler(int sig_num) {
    if (sig_num == SIGINT) {
        printf(" received an intterupt.\n");
        sleep(1);
        printf("Time to exit\n");
        exit(0);
    }
}
