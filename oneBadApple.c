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

void send_message(int node_id, int num_nodes, int fd[][2]);

int main(int argc, char *argv[]) {
    int k = atoi(argv[1]);
    int fd[k][2];
    pipe(fd[k - 1]);
    char message[128];
    char node[128];
    char header[128];
    char input[128];
    int status, destination;
    int parent_pid = getpid();
    signal(SIGINT, sig_handler);
    printf("Created node 0 (pid: %d)\n", parent_pid);
    create_nodes(1, k, fd);
    if (parent_pid == getpid()) {
        status = read(fd[k - 1][0], input, sizeof(input));
        if (status == -1) {
            perror("Failed read");
        }
        while (1) {
            printf("\nSend a message: ");
            fgets(message, sizeof(message), stdin);
            message[strlen(message) - 1] = '\0';
            printf("Send to node: ");
            fgets(node, sizeof(node), stdin);
            destination = node[0] % 48;
            while (destination >= k || destination == 0) {
                if (destination >= k) {
                    printf("That node does not exist, try again\n");
                } else {
                    printf("Already at node 0. Choose another node\n");
                }

                printf("Send to node: ");
                fgets(node, sizeof(node), stdin);
                destination = node[0] % 48;
            }
            node[1] = '\0';
            strcpy(header, node);
            strcat(header, message);

            write(fd[0][1], header, sizeof(header));
            printf("Node 0 (pid: %d) passed message to node 1\n", parent_pid);

            status = read(fd[k - 1][0], input, sizeof(input));
            if (status == -1) {
                perror("Failed read");
            }
            printf("Node 0 (pid: %d) received [%s]. Ready to send another message\n", parent_pid, input);
        }
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
        if (node_id < num_nodes) {
            close(fd[node_id - 1][1]);
            printf("Created node %d (pid: %d)\n", node_id, getpid());
        }
        create_nodes(node_id + 1, num_nodes, fd);
    } else {
        close(fd[node_id - 1][0]);
        if (node_id == num_nodes) {
            char output[128] = "All nodes created\n";
            write(fd[node_id - 1][1], output, sizeof(output));
        }
        if (node_id != 1) {
            send_message(node_id - 1, num_nodes, fd);
        }
    }
}

void send_message(int node_id, int num_nodes, int fd[][2]) {
    char input[128];
    char output[128];
    int status;
    int intended_receiver;
    int pid = getpid();

    while (1) {
        if (node_id >= num_nodes) {
            exit(0);
        }
        status = read(fd[node_id - 1][0], input, sizeof(input));
        if (status == -1) {
            perror("Failed read");
        }
        if (strcmp(input, "")) {
            intended_receiver = input[0] % 48;

            printf("Node %d (pid: %d) received a message intended for node %d\n", node_id, pid, intended_receiver);


            if (node_id == intended_receiver) {
                printf("I (node %d) am the intended recipient!\n", node_id);
                printf("The message is: %s\n", input + 1);
                printf("Node %d (pid: %d) set header to empty and passed it to node %d\n", node_id, pid,
                       (node_id + 1) % num_nodes);
                strcpy(output, "");
            } else {
                printf("Node %d (pid: %d) passed message to node %d\n", node_id, pid, node_id + 1);
                strcpy(output, input);
            }
        } else {
            printf("Node %d (pid: %d) is passing the empty header to node %d\n", node_id, pid,
                   (node_id + 1) % num_nodes);
            strcpy(output, input);
        }
        write(fd[node_id][1], output, sizeof(output));
    }
}

void sig_handler(int sig_num) {
    printf(" process pid %d received an interrupt. Exiting...\n", getpid());
    exit(0);
}
