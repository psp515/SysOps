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

struct pollfd fds[2 + MAX_CLIENTS];
char *port;
char *path;
char *clients[MAX_CLIENTS];

pthread_t ping_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int init_network_socket(char *port)
{
    int network_socket;
    struct sockaddr_in server_address;

    network_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket == -1)
    {
        perror("socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(port));
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(network_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind");
        exit(1);
    }

    return network_socket;
}

int init_local_socket(char *path)
{
     if (strlen(path) > UNIX_PATH_MAX)
    {
        printf("Path too long\n");
        exit(1);
    }

    int local_socket;
    struct sockaddr_un server_address;

    local_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (local_socket == -1)
    {
        perror("socket");
        exit(1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, path);

    if (bind(local_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind");
        exit(1);
    }

    return local_socket;
}

void kill_server()
{
    char *msg = "STOP; Server has been stopped.";

    pthread_mutex_lock(&mutex);
    for (int i = 2; i < 2 + MAX_CLIENTS; i++)
        if (fds[i].fd != -1 && send(fds[i].fd, msg, strlen(msg) + 1, 0) == -1)
            perror("SERVER STOP: Send error");
       
    shutdown(fds[0].fd, SHUT_RDWR);
    close(fds[0].fd);
    shutdown(fds[1].fd, SHUT_RDWR);
    close(fds[1].fd);

    unlink(path);
    pthread_mutex_unlock(&mutex);

    printf("\nSERVER STOP: Server is off...\n");
    fflush(stdout);
    exit(0);
}

void *pinger()
{
    char *msg = "PING;ping";
    while (1)
    {
        sleep(PING_TIMEOUT);
        pthread_mutex_lock(&mutex);
        for (int i = 2; i < 2 + MAX_CLIENTS; i++)
        {
            if (fds[i].fd != -1)
            {
                if (send(fds[i].fd, msg, strlen(msg) + 1, MSG_NOSIGNAL) == -1)
                {
                    printf("SERVER CONNECTION: Client %s disconnected\n", clients[i - 2]);
                    fflush(stdout);
                    fds[i].fd = -1;
                    clients[i - 2] = NULL;
                    continue;
                }

                char buffer[LINE_MAX];
                int res = recv(fds[i].fd, buffer, sizeof(buffer), MSG_DONTWAIT);

                if (res == 0 || (res == -1 && errno != EAGAIN && errno != EWOULDBLOCK))
                {
                    printf("SERVER CONNECTION: Client %s disconnected\n", clients[i - 2]);
                    fflush(stdout);
                    fds[i].fd = -1;
                    clients[i - 2] = NULL;
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

void handle_message(char *buffer, int client_id)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time_str[20];
    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    FILE *log_file = fopen("log.txt", "a");
    fprintf(log_file, "TIME: %s FROM: %s (ID: %d) DATA: %s\n", time_str, clients[client_id], client_id, buffer);
    fclose(log_file);
    
    char tmpBuff[LINE_MAX+1];
    memcpy(tmpBuff, buffer, strlen(buffer) + 1);

    char *command = strtok(buffer, ";");
    if (strcmp("PONG", command) == 0)
        return;

    printf("TIME: %s FROM: %s (ID: %d) DATA: %s\n", time_str, clients[client_id], client_id, tmpBuff);

    if (strcmp("LIST", command) == 0)
    {
        char msg[LINE_MAX];
        sprintf(msg, "LIST;Client list:");
        for (int i = 0; i < MAX_CLIENTS; i++)
            if (clients[i] != NULL)
                sprintf(msg + strlen(msg), "\n%s", clients[i]);

        sprintf(msg + strlen(msg), ";");

        if (send(fds[client_id + 2].fd, msg, strlen(msg) + 1, 0) == -1)
            perror("SERVER SEND: Send list error");
    }
    else if (strcmp("ALL", command) == 0)
    {
        char *message = strtok(NULL, "\n");
        char msg[LINE_MAX + 100];

        sprintf(msg, "ALL;Message from %s: %s\nTime: %s", clients[client_id], message, time_str);

        for (int i = 2; i < 2 + MAX_CLIENTS; i++)
            if (fds[i].fd != -1 && i - 2 != client_id)
                if (send(fds[i].fd, msg, strlen(msg) + 1, 0) == -1)
                    perror("SERVER SEND: Send  error");
                     
    }
    else if (strcmp("ONE", command) == 0)
    {
        char *name = strtok(NULL, ";");
        char *msg = strtok(NULL, "\n");

        char message[LINE_MAX + 100];
        sprintf(message, "ONE;Message from %s: %s\nTime: %s", clients[client_id], msg, time_str);

        int found = 0;
        for (int i = 0; i < MAX_CLIENTS; i++)
            if (clients[i] != NULL && strcmp(clients[i], name) == 0)
            {
                if (send(fds[i + 2].fd, message, strlen(message) + 1, 0) == -1)
                    perror("SERVER SEND: Send  error");
                found = 1;
                break;
            }
            
        if (!found)
        {
            char *msg = "ERROR;Client not found";
            if (send(fds[client_id + 2].fd, msg, strlen(msg) + 1, 0) == -1)
                perror("SERVER SEND: Send  error");

        }
    }
    else if (strcmp("STOP", command) == 0)
    {
        fds[client_id + 2].fd = -1;
        clients[client_id] = NULL;
    }
    else
    {
        if (send(fds[client_id + 2].fd, "ERROR;Unknown command", strlen("Unknown command") + 1, 0) == -1)
            perror("SERVER SEND: Send  error");   
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: %s <port> <path>\n", argv[0]);
        fflush(stdout);
        exit(1);
    }

    port = argv[1];
    path = argv[2];

    signal(SIGINT, kill_server);

    int network_socket = init_network_socket(port);
    int local_socket = init_local_socket(path);

    fds[0].fd = network_socket;
    fds[0].events = POLLIN;
    fds[1].fd = local_socket;
    fds[1].events = POLLIN;

    for (int i = 2; i < 2 + MAX_CLIENTS; i++)
        fds[i].fd = -1;

    printf("SERVER: Running!\n");
    fflush(stdout);

    pthread_create(&ping_thread, NULL, pinger, NULL);

    while (1)
    {
        int res = poll(fds, 2 + MAX_CLIENTS, -1);
        if (res == -1)
            perror("SERVER: Poll error");
        
        pthread_mutex_lock(&mutex);

        for (int i = 0; i < 2; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                int client_socket = accept(fds[i].fd, NULL, NULL);
                if (client_socket == -1)
                {
                    perror("SERVER: Accept error");
                    exit(1);
                }

                char buffer[LINE_MAX];
                int res = recv(client_socket, buffer, sizeof(buffer), 0);
                if (res == -1)
                {
                    perror("SERVER: Recv error");
                    exit(1);
                }

                int exists = 0;
                for(int i = 0; i < MAX_CLIENTS; i++)
                    if (clients[i - 2] != NULL && strcmp(clients[i - 2], buffer) == 0)
                    {
                        exists = 1;
                        printf("SERVER: Client with the same name already exists!\n");
                        char *response = "STOP;Client with the same name already exists!";
                        if (send(client_socket, response, strlen(response) + 1, 0) == -1)
                        {
                            perror("SERVER: Name exists.");
                            exit(1);
                        }
                        break;
                    }
            
                if (exists)
                    break;
                
                int j = 2;
                while (j < 2 + MAX_CLIENTS && fds[j].fd != -1) j++;
                
                if (j == 2 + MAX_CLIENTS)
                {
                    char *response = "STOP;Server is full! Try again later.";
                    if (send(client_socket, response, strlen(response) + 1, 0) == -1)
                        perror("SERVER: Max clients.");
                }
                else
                {
                    fds[j].fd = client_socket;
                    fds[j].events = POLLIN;
                    clients[j - 2] = malloc(strlen(buffer) + 1);
                    strcpy(clients[j - 2], buffer);

                    char *response = "OK;Client added successfully";
                    if (send(client_socket, response, strlen(response) + 1, 0) == -1)
                        perror("SERVER: Send confirm error.");
                    
                    printf("SERVER: Client %s connected!\n", clients[j - 2]);
                    fflush(stdout);
                }

                fds[i].revents = 0;
            }
        }

        for (int i = 2; i < 2 + MAX_CLIENTS; i++)
        {
            if (fds[i].revents & POLLIN && fds[i].fd != -1)
            {
                char buffer[LINE_MAX];
                int res = recv(fds[i].fd, buffer, sizeof(buffer), MSG_DONTWAIT);

                if (res == -1)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        continue;
                    
                    perror("SERVER: Recv handler error");
                }
                else if (res > 0)
                    handle_message(buffer, i - 2);

                fds[i].revents = 0;
            }
        }

        pthread_mutex_unlock(&mutex);
    }

    return 0;
}