#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include "common.h"

int socket_fd;
char *ip = NULL;
char *port = NULL;
char *unixpath = NULL;
char* clientpath = NULL;
char *name = NULL;
int totalArgs = 0;

void exit_handler()
{
    
    char * command = "STOP";
    if (send(socket_fd, command, strlen(command) + 1, 0) == -1)
    {
        printf("CLIENT: Error while sending command\n");
        exit(1);
    }

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    if (totalArgs == 3)
    {
        unlink(clientpath);
        free(unixpath);
        free(clientpath);
    }
    else
    {
        free(ip);
        free(port);
    }

    free(name);
    printf("CLIENT: Shut down\n");
    exit(0);
}

void handler(char *response)
{   
    char *command = strtok(response, ";");

    if (strcmp(command, "OK") == 0 || 
        strcmp(command, "ALL") == 0 || 
        strcmp(command, "ONE") == 0 || 
        strcmp(command, "LIST") == 0  || 
        strcmp(command, "ERROR") == 0)
    {
        command = strtok(NULL, ";");
        fprintf(stdout, "Response:\n%s\n", command);
    }
    else if (strncmp(response, "PING", 4) == 0)
    {
        char* command = "PONG;";
        if (send(socket_fd, command, strlen(command)+1, 0) == -1)
        {
            perror("CLIENT: Write error");
            exit(1);
        }
    }
    else if (strncmp(response, "STOP", 4) == 0)
    {
        printf("SERVER: Shut down\n");
        exit_handler();
    }
    else
        printf("Response: Unknown response\n");
}

void* receiver()
{
    char response[LINE_MAX];
    while (1)
    {
        memset(response, 0, LINE_MAX);

        if (recv(socket_fd, response, LINE_MAX, MSG_DONTWAIT) == -1)
            perror("CLIENT: Read error");
        
        handler(response);
    }
}

int main(int argc, char **argv)
{
    totalArgs = argc;
    if(argc == 3)
    {
        name = malloc(strlen(argv[1]) + 1);
        strcpy(name, argv[1]);
        unixpath = malloc(strlen(argv[2]) + 1);
        strcpy(unixpath, argv[2]);

        clientpath = malloc(strlen(argv[2]) + strlen(argv[1]) + 1);
        strcpy(clientpath, argv[2]);
        strcat(clientpath, argv[1]);

        socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

        if (socket_fd == -1)
        {
            printf("Error while creating socket\n");
            exit(1);
        }

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, unixpath);

        struct sockaddr_un client_addr;
        client_addr.sun_family = AF_UNIX;
        strcpy(client_addr.sun_path, clientpath);

        // Bind to local socket
        if (bind(socket_fd, (const struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
        {
            printf("Error while binding\n");
            exit(1);
        }

        if (connect(socket_fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            printf("Error while connecting\n");
            exit(1);
        }
    }
    else if(argc == 4)
    {
        name = malloc(strlen(argv[1]) + 1);
        strcpy(name, argv[1]);
        port = malloc(strlen(argv[2]) + 1);
        strcpy(port, argv[2]);
        ip = malloc(strlen(argv[3]) + 1);
        strcpy(ip, argv[3]);

        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

        if (socket_fd == -1)
        {
            printf("Error while creating socket\n");
            exit(1);
        }
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(port));
        addr.sin_addr.s_addr = inet_addr(ip);

        if (connect(socket_fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            printf("Error while connecting\n");
            exit(1);
        }

    }
    else
    {
        printf("Usage: %s <name> <localname>\n", argv[0]);
        printf("Usage: %s <name> <port> <ip>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, exit_handler);

    send(socket_fd, argv[1], strlen(argv[1]) + 1, 0);

    if (fork() == 0)
    {
        receiver();
        exit(0);
    }

    char command[LINE_MAX];
    while (fgets(command, LINE_MAX, stdin) != NULL)
    {
        command[strlen(command) - 1] = '\0';

        if (strlen(command) == 0)
            continue;

        if (strncmp(command, "STOP", 4) == 0)
        {
            exit_handler();
            break;
        }

        char *type = strtok(command, " ");
        char *message = strtok(NULL, "\n");

        if (strcmp(type, "ONE") == 0)
        {
            char *name = strtok(message, " ");
            char *msg = strtok(NULL, "\n");
            if (msg == NULL)
                msg = "";

            sprintf(message, "%s;%s", name, msg);
        }

        sprintf(command, "%s;%s", type, message);

        if (send(socket_fd, command, strlen(command) + 1, 0) == -1)
        {
            printf("CLIENT: Error while sending command\n");
            exit(1);
        }
    }

    return 0;
}
